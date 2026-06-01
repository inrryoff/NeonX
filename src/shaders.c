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
    bool custom_palette;
    int32_t off_r;
    int32_t off_g;
    int32_t off_b;
};

static const struct PresetConfig presets[] = {
    {"cyberpunk", 0,  19660, 32768, 2949120, true, 0, 0, 0},
    {"retro",     4,  13107, 52428, 0,       true, 0, 0, 0},
    {"matrix",    10, 32768, 78643, 5898240, true, 0, 0, 0},
    {"sunset",    1,  9830,  19660, 1966080, true, 0, 0, 0},
    {"vaporwave", 3,  16384, 45875, 4915200, true, 0, 0, 0},
    {"ocean",     6,  6553,  26214, 3276800, true, 0, 0, 0},
    {"forest",    2,  8192,  39321, 1638400, true, 0, 0, 0},
    {"blood",     8,  26214, 65536, 0,       true, 0, 0, 0},
    {"hacker",    0,  13107, 78643, 0,       true,  0, 137233, 0}, // Mais verde
    {"synthwave", 3,  22937, 39321, 5898240, true,  350000, 0, 150000}, // Rosa/Azul neon
    {"dracula",   1,  9830,  32768, 2949120, true,  380000, 200000, 30000} // Roxo/Rosa sombrio
};

#define NUM_PRESETS (sizeof(presets) / sizeof(presets[0]))

/** Configura o motor de renderização com base em um perfil estético (preset). */
bool shaders_set_preset(const char *preset, struct neonx_options *opts) {
    if (!preset || !opts) return false;
    for (size_t i = 0; i < NUM_PRESETS; i++) {
        if (!strcmp(preset, presets[i].name)) {
            opts->anim_mode = presets[i].anim_mode;
            
            /* Só aplica valores do preset se o usuário não tiver forçado via flag */
            if (!opts->speed_set) opts->speed_fixed = presets[i].speed;
            if (!opts->freq_set)  opts->freq_fixed = presets[i].frequency;
            if (!opts->angle_set) opts->angle_fixed = presets[i].angle;
            
            if (presets[i].custom_palette) {
                neonx_set_palette_offsets(presets[i].off_r, presets[i].off_g, presets[i].off_b);
            } else {
                neonx_reset_palette();
            }
            return true;
        }
    }
    return false;
}
