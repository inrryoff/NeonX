#include "shaders.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

int32_t freq_fixed = FLOAT_TO_FIXED(0.3f);
int32_t opacity_fixed = 0;
int32_t gradient_angle_fixed = FLOAT_TO_FIXED(-1.0f);
bool use_quantization = false;

int32_t sin_lut_fixed[LUT_SIZE];
static int32_t grad_cos_fixed = 0;
static int32_t grad_sin_fixed = 0;

#define RAD_TO_INDEX_FIXED 42722831L

void init_lut(void) { 
    for(int i = 0; i < LUT_SIZE; i++) {
        sin_lut_fixed[i] = FLOAT_TO_FIXED(sin(2.0 * M_PI * i / LUT_SIZE));
    }
}

void precalc_gradient_angle(void) {
    if (gradient_angle_fixed >= 0) {
        float rad = FIXED_TO_FLOAT(gradient_angle_fixed) * M_PI / 180.0f;
        grad_cos_fixed = FLOAT_TO_FIXED(cosf(rad));
        grad_sin_fixed = FLOAT_TO_FIXED(sinf(rad));
    }
}

uint32_t isqrt64(uint64_t n) {
    uint32_t root = 0;
    uint64_t bit = 1ULL << 62;
    while (bit > n) bit >>= 2;    
    while (bit != 0) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return root;
}

int32_t fast_dist_fixed(int32_t dx, int32_t dy) {
    int64_t dx_normal = dx >> FIXED_SHIFT;
    int64_t dy_normal = dy >> FIXED_SHIFT;
    uint64_t sq = (dx_normal * dx_normal) + (dy_normal * dy_normal);
    return (int32_t)(isqrt64(sq) << FIXED_SHIFT);
}

int32_t fast_sin_fixed(int32_t x_fixed) {
    int32_t scaled = FIXED_MUL(x_fixed, RAD_TO_INDEX_FIXED);
    int idx = ((uint32_t)scaled >> FIXED_SHIFT) & (LUT_SIZE - 1);
    int next = (idx + 1) & (LUT_SIZE - 1);
    if (idx < 0 || idx >= LUT_SIZE) idx = 0;
    if (next < 0 || next >= LUT_SIZE) next = 0;

    int32_t frac = scaled & (FIXED_ONE - 1);
    int32_t val1 = sin_lut_fixed[idx];
    int32_t val2 = sin_lut_fixed[next];
    return val1 + FIXED_MUL(frac, (val2 - val1));
}

int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase) {
    int32_t p1 = FIXED_MUL(x, FLOAT_TO_FIXED(0.15f)) + phase;
    int32_t p2 = FIXED_MUL(y, FLOAT_TO_FIXED(0.15f)) + (phase >> 1);
    return fast_sin_fixed(p1) + fast_sin_fixed(p2);
}

static inline int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase) {
    uint32_t state = (uint32_t)x * 1103515245U;
    state += (uint32_t)y * 123456789U;
    state += (uint32_t)phase * 987654321U;
    state = state ^ (state >> 16);
    return (int32_t)(state & 0x7FFFFFFF);
}

int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity) {
    int32_t p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.1f)) + FIXED_MUL(y, FLOAT_TO_FIXED(0.1f));
    int32_t pulse = FIXED_MUL(fast_sin_fixed(FIXED_MUL(phase, 3)), FLOAT_TO_FIXED(0.15f)) + FLOAT_TO_FIXED(0.85f);
    int32_t y_int = y >> FIXED_SHIFT;
    int32_t phase_int = (FIXED_MUL(phase, 5)) >> FIXED_SHIFT;
    int32_t y_mod_int = (y_int - phase_int) % 10;
    if (y_mod_int < 0) y_mod_int += 10;
    int32_t scanline = (y_mod_int < 1) ? FLOAT_TO_FIXED(0.7f) : FIXED_ONE;
    int32_t sparkle = (pseudo_rand(x, y, phase) % 100 > 98) ? FLOAT_TO_FIXED(0.5f) : FIXED_ONE;
    
    *intensity = FIXED_MUL(FIXED_MUL(pulse, scanline), sparkle);
    if (*intensity < FLOAT_TO_FIXED(0.15f)) *intensity = FLOAT_TO_FIXED(0.15f);
    return p;
}

int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity) {
    int32_t dx = x - cx;
    int32_t dy = y - cy;
    int32_t dist = fast_dist_fixed(dx, dy);    
    int32_t p = dist - phase;
    
    int32_t sin_val = fast_sin_fixed((dist >> 1) - (phase << 1));
    *intensity = (sin_val + FIXED_ONE) >> 1;
    *intensity = FIXED_MUL(*intensity, FLOAT_TO_FIXED(0.8f)) + FLOAT_TO_FIXED(0.2f);
    return p;
}

void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b) {
    if (op <= 0) return;
    if (max_dist == 0) return;
    int32_t dx = x - cx;
    int32_t dy = y - cy;
    int32_t dist = fast_dist_fixed(dx, dy);
    int32_t max_d_normal = max_dist >> FIXED_SHIFT;
    int32_t dist_normal = dist >> FIXED_SHIFT;
    if (max_d_normal == 0) return;
    int32_t dist_ratio = (dist_normal * 1000) / max_d_normal;
    int32_t decay = (dist_ratio * op) / 1000;
    int32_t factor = 1000 - decay;
    if (factor < 0) factor = 0;
    if (factor > 1000) factor = 1000;
    *r = (int)(((int64_t)*r * factor) / 1000);
    *g = (int)(((int64_t)*g * factor) / 1000);
    *b = (int)(((int64_t)*b * factor) / 1000);
    if (*r < 0) *r = 0; else if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; else if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; else if (*b > 255) *b = 255;
}

void get_color_fast(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    int32_t p;
    int32_t intensity = FIXED_ONE;
    
    switch(mode) {  
        case 1: p = shader_sunset_fixed(x, y, phase); break;
        case 2: p = phase; break;
        case 3: p = (y >> 1) + phase; break;
        case 4: p = FIXED_MUL(y, FLOAT_TO_FIXED(0.8f)) + FIXED_MUL(fast_sin_fixed(FIXED_MUL(x, FLOAT_TO_FIXED(0.3f))), FLOAT_TO_FIXED(3.0f)) + phase; break;
        case 5: p = fast_dist_fixed(x - cx, y - cy) - phase; break;  
        case 6: p = FIXED_MUL(FIXED_MUL(fast_sin_fixed(FIXED_MUL(x, FLOAT_TO_FIXED(0.2f))), fast_sin_fixed(FIXED_MUL(y, FLOAT_TO_FIXED(0.2f)) + FIXED_PI_2)), FLOAT_TO_FIXED(10.0f)) + phase; break;  
        case 7: { 
            p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.1f)); 
            int32_t diff = x > cx ? x - cx : cx - x;
            int32_t ratio = (cx == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cx);
            intensity = FIXED_ONE - FIXED_MUL(ratio, FLOAT_TO_FIXED(0.8f)); 
            if(intensity < FLOAT_TO_FIXED(0.2f)) intensity = FLOAT_TO_FIXED(0.2f); 
            break;
        }  
        case 8: { 
            p = phase + FIXED_MUL(y, FLOAT_TO_FIXED(0.2f)); 
            int32_t diff = y > cy ? y - cy : cy - y;
            int32_t ratio = (cy == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cy);
            intensity = FIXED_ONE - FIXED_MUL(ratio, FLOAT_TO_FIXED(0.7f)); 
            if(intensity < FLOAT_TO_FIXED(0.3f)) intensity = FLOAT_TO_FIXED(0.3f); 
            break;
        }  
        case 9: p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.1f)); intensity = ((x >> FIXED_SHIFT) % 2 == 0) ? FIXED_ONE : FLOAT_TO_FIXED(0.6f); break;
        case 10: p = shader_matrix_fixed(x, y, phase, &intensity); break;  
        case 11: p = shader_pulse_fixed(x, y, cx, cy, phase, &intensity); break;
        default: 
            p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.2f)) + FIXED_MUL(y, FLOAT_TO_FIXED(0.1f)); 
            if (gradient_angle_fixed >= 0) {
                p += FIXED_MUL(grad_cos_fixed, x) + FIXED_MUL(grad_sin_fixed, y); 
            } 
            intensity = FIXED_ONE;
            break;
    }
    int32_t base_phase = FIXED_MUL(freq_fixed, p);
    int32_t sin_r = fast_sin_fixed(base_phase);
    int32_t sin_g = fast_sin_fixed(base_phase + FLOAT_TO_FIXED(2.094f));
    int32_t sin_b = fast_sin_fixed(base_phase + FLOAT_TO_FIXED(4.188f));
    int raw_r = ((sin_r * 127) >> FIXED_SHIFT) + 128;
    int raw_g = ((sin_g * 127) >> FIXED_SHIFT) + 128;
    int raw_b = ((sin_b * 127) >> FIXED_SHIFT) + 128;
    if (mode == 0) {
        *r = raw_r;
        *g = raw_g;
        *b = raw_b;
    } else {
        *r = (raw_r * intensity) >> FIXED_SHIFT;
        *g = (raw_g * intensity) >> FIXED_SHIFT;
        *b = (raw_b * intensity) >> FIXED_SHIFT;
    }

    if (use_quantization) {
        *r &= 0xF8; *g &= 0xF8; *b &= 0xF8;
    }
    if (*r < 0) *r = 0; if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; if (*b > 255) *b = 255;
    if (mode != 0) {
        apply_border_opacity_fixed(x, y, cx, cy, max_dist, opacity_fixed, r, g, b);
    }
}

void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed) {
    if (!strcmp(preset,"cyberpunk")) { 
        *anim_mode = 0; *speed_fixed = FLOAT_TO_FIXED(0.3); freq_fixed = FLOAT_TO_FIXED(0.5); gradient_angle_fixed = FLOAT_TO_FIXED(45.0);
    } else if (!strcmp(preset,"retro")) { 
        *anim_mode = 4; *speed_fixed = FLOAT_TO_FIXED(0.2); freq_fixed = FLOAT_TO_FIXED(0.8); gradient_angle_fixed = FLOAT_TO_FIXED(0.0);
    } else if (!strcmp(preset,"matrix")) { 
        *anim_mode = 10; *speed_fixed = FLOAT_TO_FIXED(0.5); freq_fixed = FLOAT_TO_FIXED(1.2); gradient_angle_fixed = FLOAT_TO_FIXED(90.0);
    } else if (!strcmp(preset,"sunset")) { 
        *anim_mode = 1; *speed_fixed = FLOAT_TO_FIXED(0.15); freq_fixed = FLOAT_TO_FIXED(0.3); gradient_angle_fixed = FLOAT_TO_FIXED(30.0);
    }
}

void shaders_set_frequency(int32_t freq) {
    freq_fixed = freq;
}

void shaders_set_gradient_angle(int32_t angle) {
    gradient_angle_fixed = angle;
}

void shaders_set_opacity_from_string(const char *o_val) {
    int32_t int_part = 0, frac_part = 0;
    int frac_len = 0;
    if (*o_val == '-') o_val++;
    while (*o_val >= '0' && *o_val <= '9') {
        int_part = int_part * 10 + (*o_val - '0');
        o_val++;
    }
    if (*o_val == '.') {
        o_val++;
        while (*o_val >= '0' && *o_val <= '9' && frac_len < 3) {
            frac_part = frac_part * 10 + (*o_val - '0');
            frac_len++;
            o_val++;
        }
    }
    while (frac_len < 3) { frac_part *= 10; frac_len++; }

    opacity_fixed = (int_part * 1000) + frac_part;
    if (opacity_fixed < 0) opacity_fixed = 0;
    if (opacity_fixed > 1000) opacity_fixed = 1000;
}

void shaders_finalize_setup(void) {
    precalc_gradient_angle();
}