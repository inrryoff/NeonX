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

void get_color_fast(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    neonx_get_color(x, y, mode, cx, cy, max_dist, phase, r, g, b);
}

struct PresetConfig {
    const char *name;
    int anim_mode;
    int32_t speed;
    int32_t frequency;
    int32_t angle;
};

static const struct PresetConfig presets[] = {
    {"cyberpunk", 0,  19660, 32768, 2949120},  // 0.3, 0.5, 45.0
    {"retro",     4,  13107, 52428, 0},        // 0.2, 0.8, 0.0
    {"matrix",    10, 32768, 78643, 5898240},  // 0.5, 1.2, 90.0
    {"sunset",    1,  9830,  19660, 1966080}   // 0.15, 0.3, 30.0
};
#define NUM_PRESETS (sizeof(presets) / sizeof(presets[0]))

void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed) {
    for (size_t i = 0; i < NUM_PRESETS; i++) {
        if (!strcmp(preset, presets[i].name)) {
            *anim_mode = presets[i].anim_mode;
            *speed_fixed = presets[i].speed;
            neonx_set_frequency(presets[i].frequency);
            neonx_set_gradient_angle(presets[i].angle);
            return;
        }
    }
}

void shaders_set_opacity_from_string(const char *o_val) {
    // Custom str_to_fixed logic to avoid strtod
    int32_t val_fixed = 0;
    int32_t fraction = 0;
    int divisor = 1;
    bool in_fraction = false;
    const char *s = o_val;
    
    while (*s) {
        if (*s == '.') {
            in_fraction = true;
        } else if (*s >= '0' && *s <= '9') {
            if (in_fraction) {
                fraction = fraction * 10 + (*s - '0');
                divisor *= 10;
            } else {
                val_fixed = val_fixed * 10 + (*s - '0');
            }
        } else {
            fprintf(stderr, "%s", MSG(MSG_ERR_INVALID_NUMBER));
            exit(3);
        }
        s++;
    }
    int32_t result = (val_fixed * 1000) + ((fraction * 1000) / divisor);
    if (result < 0 || result > 1000) {
        fprintf(stderr, "%s", MSG(MSG_ERR_INVALID_NUMBER));
        exit(3);
    }
    neonx_set_opacity(result);
}

void shaders_finalize_setup(void) {
    // neonx_set_gradient_angle já chama precalc
}
