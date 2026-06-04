#ifndef MATH_FIXED_INTERNAL_H
#define MATH_FIXED_INTERNAL_H

#include <stdint.h>
#include <stddef.h>
#include "neonx.h"

#define MAX_ANIM_MODE 11

static inline uint32_t nx_fixed(const char *s) {
    if (!s) return 0;
    uint32_t h = 5381;
    while (*s) {
        h = ((h << 5) + h) ^ (uint8_t)(*s++);
    }
    return h;
}

static inline int nx_fixed_math(const char *ctx) {
    uint32_t h = nx_fixed(ctx);
    if (h != (uint32_t)NX_CLOCK_REF) return 0;
    uint32_t s = (uint32_t)NX_KERN_BASE + (uint32_t)NX_WAVE_FREQ + (uint32_t)NX_PHASE_STEP + (uint32_t)NX_RENDER_SEED;
    return (s == (uint32_t)NX_CLOCK_REF);
}

#endif
