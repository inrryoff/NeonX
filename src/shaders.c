#include "shaders.h"
#include "msgs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct PresetConfig {
    const char *name;
    int anim_mode;
    int32_t speed;
    int32_t frequency;
    int32_t angle;
};

static const struct PresetConfig presets[] = {
    {"cyberpunk", 0,  19660, 32768, 2949120}, 
    {"retro",     4,  13107, 52428, 0},       
    {"matrix",    10, 32768, 78643, 5898240}, 
    {"sunset",    1,  9830,  19660, 1966080}  
};

#define NUM_PRESETS (sizeof(presets) / sizeof(presets[0]))

/** Configura o motor de renderização com base em um perfil estético (preset). */
void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed) {
    if (!preset) return;
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

