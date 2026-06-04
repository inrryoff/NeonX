#include "neonx.h"
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
    {"cyberpunk", 0,  19660, 32768, 2949120, true, 0, 137233, 274466},
    {"retro",     4,  13107, 52428, 0,       true, 274466, 137233, 0},
    {"matrix",    10, 6553,  78643, 5898240, true, 0, 137233, 0},
    {"sunset",    1,  9830,  19660, 1966080, true, 274466, 68616, 0},
    {"vaporwave", 3,  16384, 45875, 4915200, true, 200000, 0, 300000},
    {"ocean",     6,  6553,  26214, 3276800, true, 0, 100000, 300000},
    {"forest",    2,  8192,  39321, 1638400, true, 50000, 200000, 0},
    {"blood",     8,  26214, 65536, 0,       true, 250000, 0, 0},
    {"hacker",    0,  13107, 78643, 0,       true, 0, 137233, 0},
    {"synthwave", 3,  22937, 39321, 5898240, true, 350000, 0, 150000},
    {"dracula",    1,  9830,  32768, 2949120, true,  380000, 200000, 30000},
    {"aurora",     5,  6553,  26214, 0,       true,  0,      180000, 320000},
    {"neon_tokyo", 4,  19660, 52428, 0,       true,  320000, 0,      280000},
    {"lava",       8,  22937, 39321, 0,       true,  300000, 60000,  0},
    {"ice",        11, 6553,  19660, 0,       true,  50000,  200000, 350000},
    {"fire",       3,  26214, 78643, 0,       true,  350000, 120000, 0},
    {"galaxy",     5,  9830,  32768, 0,       true,  80000,  0,      300000},
    {"toxic",      10, 16384, 65536, 0,       true,  80000,  300000, 0},
    {"midnight",   0,  6553,  19660, 2621440, true,  30000,  0,      200000},
    {"rose",       1,  13107, 39321, 0,       true,  300000, 50000,  150000},
    {"vapor2",     6,  19660, 45875, 0,       true,  250000, 100000, 350000}
};

#define NUM_PRESETS (sizeof(presets) / sizeof(presets[0]))

bool shaders_set_preset(const char *preset, struct neonx_options *opts) {
    if (!preset || !opts) return false;
    for (size_t i = 0; i < NUM_PRESETS; i++) {
        if (!strcmp(preset, presets[i].name)) {
            opts->anim_mode = presets[i].anim_mode;

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
