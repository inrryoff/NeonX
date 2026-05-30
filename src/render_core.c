#include "render_core.h"
#include "math_fixed.h"
#include "shader_effects.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int32_t freq_fixed = 19660; 
static int32_t opacity_fixed = 0;
static int32_t gradient_angle_fixed = -65536; 
static bool use_quantization = false;

static int32_t grad_cos_fixed = 0;
static int32_t grad_sin_fixed = 0;

/** Pré-calcula os valores de seno e cosseno para o ângulo do gradiente. */
static void precalc_gradient_angle(void) {
    if (gradient_angle_fixed >= 0) {
        double rad = ((double)gradient_angle_fixed / 65536.0) * M_PI / 180.0;
        grad_cos_fixed = (int32_t)(cos(rad) * 65536.0);
        grad_sin_fixed = (int32_t)(sin(rad) * 65536.0);
    }
}

/** Aplica decaimento de opacidade nas bordas com base na distância radial. */
void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b) {
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

/** Calcula a cor final (RGB) para uma coordenada específica e modo de animação. */
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

/** Define a frequência global das oscilações de cor. */
void neonx_set_frequency(int32_t freq) {
    freq_fixed = freq;
}

/** Define e processa o ângulo de inclinação do gradiente. */
void neonx_set_gradient_angle(int32_t angle) {
    gradient_angle_fixed = angle;
    precalc_gradient_angle();
}

/** Ajusta o nível de opacidade aplicado às bordas da renderização. */
void neonx_set_opacity(int32_t op) {
    opacity_fixed = op;
}

/** Ativa ou desativa a quantização de cores para efeito retrô. */
void neonx_set_quantization(bool enable) {
    use_quantization = enable;
}

#include <wchar.h>
#include <string.h>

/** Renderiza uma linha completa de caracteres aplicando cores dinâmicas. */
void neonx_render_line(wchar_t *line, int32_t y_fixed, int32_t phase, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, RenderDriver *driver) {
    if (!line) return;
    size_t line_len = wcslen(line);
    int last_r = -1, last_g = -1, last_b = -1;

    for (size_t x = 0; x < line_len; x++) {
        int r = 0, g = 0, b = 0;
        neonx_get_color((int32_t)x << FIXED_SHIFT, y_fixed, mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);

        if (line[x] != L' ' && line[x] != L'\t' && line[x] != L'\r' && line[x] != L'\n') {
            if (r != last_r || g != last_g || b != last_b) {
                driver->set_color(driver, r, g, b);
                last_r = r; last_g = g; last_b = b;
            }
        }
        driver->put_char(driver, line[x]);
    }
    driver->reset_color(driver);
}


