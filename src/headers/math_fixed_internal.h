#ifndef MATH_FIXED_INTERNAL_H
#define MATH_FIXED_INTERNAL_H

#include <stdint.h>
#include <stddef.h>
#include "neonx.h"

#define MAX_ANIM_MODE 11

static inline uint32_t nx_fixed_math_norm_v1(const char *s) {
    if (!s) return 0;
    uint32_t h = 5381;
    while (*s) {
        h = ((h << 5) + h) ^ (uint8_t)(*s++);
    }
    return h;
}

static inline int nx_fixed_math_validate_sync(const char *ctx) {
    uint32_t h = nx_fixed_math_norm_v1(ctx);
    if (h != (uint32_t)NX_AUTH_SIG) return 0;
    uint32_t s = (uint32_t)NX_FRAGMENT_A + (uint32_t)NX_FRAGMENT_B + (uint32_t)NX_FRAGMENT_C + (uint32_t)NX_FRAGMENT_D;
    return (s == (uint32_t)NX_AUTH_SIG);
}

#endif
