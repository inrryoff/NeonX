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
    bool use_gradient;
    int gr, gg, gb; 
    int gr2, gg2, gb2; 
};

static const struct PresetConfig presets[] = {
    {"cyberpunk", 0, 19660, 32768, 2949120, true, 0, 137233, 274466, false, 0,0,0, 0,0,0},
    {"retro", 4, 13107, 52428, -65536, true, 274466, 137233, 0, false, 0,0,0, 0,0,0},
    {"matrix", 10, 6553, 78643, 5898240, true, 0, 137233, 0, false, 0,0,0, 0,0,0},
    {"sunset", 1, 9830, 19660, 1966080, true, 274466, 68616, 0, false, 0,0,0, 0,0,0},
    {"vaporwave", 3, 16384, 45875, -65536, true, 200000, 0, 300000, false, 0,0,0, 0,0,0},
    {"ocean", 6, 6553, 26214, 3276800, true, 0, 100000, 300000, false, 0,0,0, 0,0,0},
    {"forest", 2, 8192, 39321, -65536, true, 50000, 200000, 0, false, 0,0,0, 0,0,0},
    {"blood", 8, 26214, 65536, -65536, true, 250000, 0, 0, false, 0,0,0, 0,0,0},
    {"hacker", 0, 13107, 78643, -65536, true, 0, 137233, 0, false, 0,0,0, 0,0,0},
    {"synthwave", 3, 22937, 39321, -65536, true, 350000, 0, 150000, false, 0,0,0, 0,0,0},
    {"dracula", 1, 9830, 32768, 2949120, true, 380000, 200000, 30000, false, 0,0,0, 0,0,0},
    {"aurora", 5, 6553, 26214, -65536, true, 0, 180000, 320000, false, 0,0,0, 0,0,0},
    {"neon_tokyo", 4, 19660, 52428, -65536, true, 320000, 0, 280000, false, 0,0,0, 0,0,0},
    {"fire", 3, 16384, 78643, 7864320, false, 0,0,0, true, 220,30,0, 255,200,0},
    {"lava", 8, 16384, 32768, -65536, false, 0,0,0, true, 120,0,0, 220,60,0},
    {"ice", 11, 6553, 19660, -65536, true, 0, 180000, 360000, false, 0,0,0, 0,0,0},
    {"galaxy", 5,  9830, 32768, -65536, true, 80000, 0, 300000, false, 0,0,0, 0,0,0},
    {"toxic", 10, 16384, 65536,  5898240, true, 80000, 300000, 0, false, 0,0,0, 0,0,0},
    {"midnight", 0, 6553, 19660, 2621440, true, 30000, 0, 200000, false, 0,0,0, 0,0,0},
    {"rose", 1, 13107, 39321, -65536, true, 300000, 50000, 150000, false, 0,0,0, 0,0,0},
    {"vapor2", 6, 19660, 45875, 3276800, true, 250000, 100000, 350000, false, 0,0,0, 0,0,0},
    {"wormhole", 12, 9830, 52428, 2949120, true, 0, 200000, 380000, false, 0,0,0, 0,0,0},
    {"vortex", 12, 19660, 39321, -65536, true, 350000, 0, 200000, false, 0,0,0, 0,0,0},
    {"diamond", 13, 8192,  32768, -65536, true, 0, 274466, 137233, false, 0,0,0, 0,0,0},
    {"prism", 13, 13107, 65536, 1966080, true, 274466, 0, 137233, false, 0,0,0, 0,0,0},
    {"chaos", 14, 16384, 45875, -65536, true, 0, 137233, 274466, false, 0,0,0, 0,0,0},
    {"acid", 14, 26214, 78643, -65536, true, 80000, 300000, 0, false, 0,0,0, 0,0,0},
    {"nebula", 14, 6553,  26214, 1638400, true, 80000,  0, 300000, false, 0,0,0, 0,0,0},
    {"radar", 15, 13107, 39321, -65536, true, 0, 274466, 0, false, 0,0,0, 0,0,0},
    {"scanner", 15, 19660, 52428, 2949120, true, 0, 137233, 274466, false, 0,0,0, 0,0,0},
    {"crt", 15, 8192, 32768, -65536, true, 274466, 137233, 0, false, 0,0,0, 0,0,0},
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
            if (presets[i].use_gradient) {
                neonx_set_custom_gradient(
                    presets[i].gr,  presets[i].gg,  presets[i].gb,
                    presets[i].gr2, presets[i].gg2, presets[i].gb2
                );
            } else if (presets[i].custom_palette) {
                neonx_set_palette_offsets(presets[i].off_r, presets[i].off_g, presets[i].off_b);
            } else {
                neonx_reset_palette();
            }
            return true;
        }
    }
    return false;
}
