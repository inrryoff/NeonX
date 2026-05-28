#include "shaders.h"
#include "msgs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_lut(void) {
    neonx_init_lut();
}

void precalc_gradient_angle(void) {
    // Agora tratado pelo setter no core ou finalize
}

uint32_t isqrt64(uint64_t n) {
    return neonx_isqrt64(n);
}

int32_t fast_dist_fixed(int32_t dx, int32_t dy) {
    return neonx_fast_dist_fixed(dx, dy);
}

int32_t fast_sin_fixed(int32_t x_fixed) {
    return neonx_fast_sin_fixed(x_fixed);
}

void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b) {
    // Esta função era interna no core agora, mas podemos expor se necessário ou apenas chamar get_color
    // Na verdade, shaders.c original tinha essa função pública.
    // Vamos manter a compatibilidade mas como ela é usada apenas internamente no get_color_fast original,
    // podemos deixá-la aqui como um wrapper que usa o core se o core a exportasse.
    // Como o core não a exporta no .h (é estática lá), vamos manter uma cópia ou mover para o .h do core.
    // Decisão: Mover apply_border_opacity_fixed para neonx_core.h se for realmente necessária externamente.
    // No NeonX original, ela era usada em get_color_fast.
    // Vou deixar o wrapper vazio ou chamar neonx_get_color com parâmetros específicos se for o caso.
    // Na verdade, vamos apenas chamar neonx_get_color que já faz isso.
}

void get_color_fast(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    neonx_get_color(x, y, mode, cx, cy, max_dist, phase, r, g, b);
}

void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed) {
    if (!strcmp(preset,"cyberpunk")) {
        *anim_mode = 0;
        *speed_fixed = FLOAT_TO_FIXED(0.3);
        neonx_set_frequency(FLOAT_TO_FIXED(0.5));
        neonx_set_gradient_angle(FLOAT_TO_FIXED(45.0));
    } else if (!strcmp(preset,"retro")) {
        *anim_mode = 4;
        *speed_fixed = FLOAT_TO_FIXED(0.2);
        neonx_set_frequency(FLOAT_TO_FIXED(0.8));
        neonx_set_gradient_angle(FLOAT_TO_FIXED(0.0));
    } else if (!strcmp(preset,"matrix")) {
        *anim_mode = 10;
        *speed_fixed = FLOAT_TO_FIXED(0.5);
        neonx_set_frequency(FLOAT_TO_FIXED(1.2));
        neonx_set_gradient_angle(FLOAT_TO_FIXED(90.0));
    } else if (!strcmp(preset,"sunset")) {
        *anim_mode = 1;
        *speed_fixed = FLOAT_TO_FIXED(0.15);
        neonx_set_frequency(FLOAT_TO_FIXED(0.3));
        neonx_set_gradient_angle(FLOAT_TO_FIXED(30.0));
    }
}

void shaders_set_frequency(int32_t freq) {
    neonx_set_frequency(freq);
}

void shaders_set_gradient_angle(int32_t angle) {
    neonx_set_gradient_angle(angle);
}

void shaders_set_quantization(bool enable) {
    neonx_set_quantization(enable);
}

void shaders_set_opacity_from_string(const char *o_val) {
    char *endptr;
    double val = strtod(o_val, &endptr);
    if (*endptr != '\0' || o_val == endptr || val < 0.0 || val > 1.0) {
        fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), "-o", o_val);
        exit(3);
    }
    neonx_set_opacity((int32_t)(val * 1000.0));
}

void shaders_finalize_setup(void) {
    // neonx_set_gradient_angle já chama precalc
}
