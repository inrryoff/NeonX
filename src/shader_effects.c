#include "shader_effects.h"
#include "math_fixed.h"

/** Implementa o efeito visual 'sunset' utilizando ondas senoidais compostas. */
int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase) {
    int32_t p1 = FIXED_MUL(x, 9830) + phase;
    int32_t p2 = FIXED_MUL(y, 9830) + (phase >> 1);
    return neonx_fast_sin_fixed(p1) + neonx_fast_sin_fixed(p2);
}

/** Gera um valor pseudo-aleatório rápido para efeitos de cintilação. */
int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase) {
    uint32_t state = (uint32_t)x * 1103515245U;
    state += (uint32_t)y * 123456789U;
    state += (uint32_t)phase * 987654321U;
    state = state ^ (state >> 16);
    return (int32_t)(state & 0x7FFFFFFF);
}

/** Implementa o efeito 'matrix' com simulação de scanlines e brilho aleatório. */
int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity) {
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

/** Implementa o efeito 'pulse' com ondas radiais a partir do centro. */
int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity) {
    int32_t dx = x - cx;
    int32_t dy = y - cy;
    int32_t dist = neonx_fast_dist_fixed(dx, dy);
    int32_t p = dist - phase;
    int32_t sin_val = neonx_fast_sin_fixed((dist >> 1) - (phase << 1));
    *intensity = (sin_val + FIXED_ONE) >> 1;
    *intensity = FIXED_MUL(*intensity, 52428) + 13107;
    return p;
}

