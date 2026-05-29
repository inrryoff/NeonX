#include "neonx_core.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h> 
#endif

// Tamanho da LUT
#define LUT_SIZE 4096

// Constante para converter radianos diretamente num índice da nossa tabela LUT
// Matemáticamente: (LUT_SIZE / (2 * PI)) * FIXED_ONE
#define RAD_TO_INDEX_FIXED 42722831L

// Variáveis globais estáticas (encapsuladas)
static int32_t freq_fixed = 19660; // 0.3 em ponto fixo
static int32_t opacity_fixed = 0;
static int32_t gradient_angle_fixed = -65536; // -1.0 em ponto fixo
static bool use_quantization = false;
static int32_t sin_lut_fixed[LUT_SIZE];

// Componentes trigonométricas pré-calculadas do ângulo
static int32_t grad_cos_fixed = 0;
static int32_t grad_sin_fixed = 0;

uint32_t secure_random_u32(void) {
#ifndef _WIN32
    return (uint32_t)time(NULL) ^ (uint32_t)getpid();
#else
    return (uint32_t)time(NULL) ^ (uint32_t)_getpid();
#endif
}

void neonx_init_lut(void) {
    for(int i = 0; i < LUT_SIZE; i++) {
        sin_lut_fixed[i] = (int32_t)(sin(2.0 * M_PI * i / LUT_SIZE) * 65536.0);
    }
}

static void precalc_gradient_angle(void) {
    if (gradient_angle_fixed >= 0) {
        double rad = ((double)gradient_angle_fixed / 65536.0) * M_PI / 180.0;
        grad_cos_fixed = (int32_t)(cos(rad) * 65536.0);
        grad_sin_fixed = (int32_t)(sin(rad) * 65536.0);
    }
}

uint32_t neonx_isqrt64(uint64_t n) {
    uint32_t root = 0;
    uint64_t bit = 1ULL << 62;
    while (bit > n) bit >>= 2;
    while (bit != 0) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (uint32_t)((root >> 1) + bit);
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return root;
}

int32_t neonx_fast_dist_fixed(int32_t dx, int32_t dy) {
    int64_t dx_normal = dx >> FIXED_SHIFT;
    int64_t dy_normal = dy >> FIXED_SHIFT;
    uint64_t sq = (uint64_t)((dx_normal * dx_normal) + (dy_normal * dy_normal));
    return (int32_t)(neonx_isqrt64(sq) << FIXED_SHIFT);
}

int32_t neonx_fast_sin_fixed(int32_t x_fixed) {
    int32_t scaled = FIXED_MUL(x_fixed, RAD_TO_INDEX_FIXED);
    int idx = ((uint32_t)scaled >> FIXED_SHIFT) & (LUT_SIZE - 1);
    int next = (idx + 1) & (LUT_SIZE - 1);
    int32_t frac = scaled & (FIXED_ONE - 1);
    int32_t val1 = sin_lut_fixed[idx];
    int32_t val2 = sin_lut_fixed[next];
    return val1 + FIXED_MUL(frac, (val2 - val1));
}

// --- SHADERS INTERNOS ---

static int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase) {
    int32_t p1 = FIXED_MUL(x, 9830) + phase;
    int32_t p2 = FIXED_MUL(y, 9830) + (phase >> 1);
    return neonx_fast_sin_fixed(p1) + neonx_fast_sin_fixed(p2);
}

static inline int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase) {
    uint32_t state = (uint32_t)x * 1103515245U;
    state += (uint32_t)y * 123456789U;
    state += (uint32_t)phase * 987654321U;
    state = state ^ (state >> 16);
    return (int32_t)(state & 0x7FFFFFFF);
}

static int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity) {
    int32_t p = phase + FIXED_MUL(x, 6553) + FIXED_MUL(y, 6553);
    int32_t pulse = FIXED_MUL(neonx_fast_sin_fixed(FIXED_MUL(phase, 3)), 9830) + 55705;
    int32_t y_int = y >> FIXED_SHIFT;
    int32_t phase_int = (FIXED_MUL(phase, 5)) >> FIXED_SHIFT;
    int32_t y_mod_int = (y_int - phase_int) % 10;
    if (y_mod_int < 0) y_mod_int += 10;
    int32_t scanline = (y_mod_int < 1) ? 45875 : FIXED_ONE;
    int32_t sparkle = (pseudo_rand(x, y, phase) % 100 > 98) ? 32768 : FIXED_ONE;
    *intensity = FIXED_MUL(FIXED_MUL(pulse, scanline), sparkle);
    if (*intensity < 9830) *intensity = 9830;
    return p;
}

static int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity) {
    int32_t dx = x - cx;
    int32_t dy = y - cy;
    int32_t dist = neonx_fast_dist_fixed(dx, dy);
    int32_t p = dist - phase;
    int32_t sin_val = neonx_fast_sin_fixed((dist >> 1) - (phase << 1));
    *intensity = (sin_val + FIXED_ONE) >> 1;
    *intensity = FIXED_MUL(*intensity, 52428) + 13107;
    return p;
}

static void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b) {
    if (op <= 0 || max_dist <= 0) return;
    int32_t dist = neonx_fast_dist_fixed(x - cx, y - cy);
    int32_t ratio = (int32_t)(((int64_t)dist * FIXED_ONE) / max_dist);
    int32_t decay = FIXED_MUL(ratio, op);
    int32_t factor = FIXED_ONE - decay;
    if (factor < 0) factor = 0;
    if (factor > FIXED_ONE) factor = FIXED_ONE;

    *r = (*r * factor) >> FIXED_SHIFT;
    *g = (*g * factor) >> FIXED_SHIFT;
    *b = (*b * factor) >> FIXED_SHIFT;

    if (*r < 0) *r = 0; else if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; else if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; else if (*b > 255) *b = 255;
}

void neonx_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    int32_t p;
    int32_t intensity = FIXED_ONE;

    switch(mode) {
    case 1: p = shader_sunset_fixed(x, y, phase); break;
    case 2: p = phase; break;
    case 3: p = (y >> 1) + phase; break;
    case 4: p = FIXED_MUL(y, 52428) + FIXED_MUL(neonx_fast_sin_fixed(FIXED_MUL(x, 19660)), 196608) + phase; break;
    case 5: p = neonx_fast_dist_fixed(x - cx, y - cy) - phase; break;
    case 6: p = FIXED_MUL(FIXED_MUL(neonx_fast_sin_fixed(FIXED_MUL(x, 13107)), neonx_fast_sin_fixed(FIXED_MUL(y, 13107) + FIXED_PI_2)), 655360) + phase; break;
    case 7: {
        p = phase + FIXED_MUL(x, 6553);
        int32_t diff = x > cx ? x - cx : cx - x;
        int32_t ratio = (cx == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cx);
        intensity = FIXED_ONE - FIXED_MUL(ratio, 52428);
        if(intensity < 13107) intensity = 13107;
        break;
    }
    case 8: {
        p = phase + FIXED_MUL(y, 13107);
        int32_t diff = y > cy ? y - cy : cy - y;
        int32_t ratio = (cy == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cy);
        intensity = FIXED_ONE - FIXED_MUL(ratio, 45875);
        if(intensity < 19660) intensity = 19660;
        break;
    }
    case 9:
        p = phase + FIXED_MUL(x, 6553);
        intensity = ((x >> FIXED_SHIFT) % 2 == 0) ? FIXED_ONE : 39321;
        break;
    case 10: p = shader_matrix_fixed(x, y, phase, &intensity); break;
    case 11: p = shader_pulse_fixed(x, y, cx, cy, phase, &intensity); break;
    default:
        p = phase + FIXED_MUL(x, 13107) + FIXED_MUL(y, 6553);
        if (gradient_angle_fixed >= 0) {
            p += FIXED_MUL(grad_cos_fixed, x) + FIXED_MUL(grad_sin_fixed, y);
        }
        intensity = FIXED_ONE;
        break;
    }

    int32_t base_phase = FIXED_MUL(freq_fixed, p);
    int32_t sin_r = neonx_fast_sin_fixed(base_phase);
    int32_t sin_g = neonx_fast_sin_fixed(base_phase + 137233);
    int32_t sin_b = neonx_fast_sin_fixed(base_phase + 274466);

    int raw_r = ((sin_r * 127) >> FIXED_SHIFT) + 128;
    int raw_g = ((sin_g * 127) >> FIXED_SHIFT) + 128;
    int raw_b = ((sin_b * 127) >> FIXED_SHIFT) + 128;

    if (mode == 0) {
        *r = raw_r; *g = raw_g; *b = raw_b;
    } else {
        *r = (raw_r * intensity) >> FIXED_SHIFT;
        *g = (raw_g * intensity) >> FIXED_SHIFT;
        *b = (raw_b * intensity) >> FIXED_SHIFT;
    }

    if (use_quantization) {
        *r &= 0xF8; *g &= 0xF8; *b &= 0xF8;
    }

    if (*r < 0) *r = 0; else if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; else if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; else if (*b > 255) *b = 255;

    if (mode != 0) {
        apply_border_opacity_fixed(x, y, cx, cy, max_dist, opacity_fixed, r, g, b);
    }
}

// --- SETTERS ---

void neonx_set_frequency(int32_t freq) {
    freq_fixed = freq;
}

void neonx_set_gradient_angle(int32_t angle) {
    gradient_angle_fixed = angle;
    precalc_gradient_angle();
}

void neonx_set_opacity(int32_t op) {
    opacity_fixed = op;
}

void neonx_set_quantization(bool enable) {
    use_quantization = enable;
}
