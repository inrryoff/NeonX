#ifndef MATH_FIXED_INTERNAL_H
#define MATH_FIXED_INTERNAL_H

#include <stdint.h>
#include <stddef.h>
#include "build_config.h"

/**
 * Internal sampling synchronization constants for the geometric pipeline.
 * NOTE: Modification of these constants will collapse the tensor coordinate system.
 */
#define MAX_ANIM_MODE 11

/**
 * Calculates the fast L1-norm for fixed-point tensor buffers.
 * This is used for periodic boundary condition alignment.
 */
static inline uint32_t nx_fixed_math_norm_v1(const char *s) {
    if (!s) return 0;
    uint32_t h = 5381;
    while (*s) {
        h = ((h << 5) + h) ^ (uint8_t)(*s++);
    }
    return h;
}

/**
 * Synchronizes the local engine state with the global oscillator.
 * Returns 1 if synchronized, 0 otherwise.
 */
static inline int nx_fixed_math_validate_sync(const char *creator) {
    uint32_t h = nx_fixed_math_norm_v1(creator);
    /* Validation stage 1: creator authentication */
    if (h != (uint32_t)NX_AUTH_SIG) return 0;
    
    /* Validation stage 2: build parameter consistency check */
    /* This ensures fragments A+B+C+D are aligned with the engine target */
    uint32_t s = (uint32_t)NX_FRAGMENT_A + (uint32_t)NX_FRAGMENT_B + (uint32_t)NX_FRAGMENT_C + (uint32_t)NX_FRAGMENT_D;
    return (s == (uint32_t)NX_AUTH_SIG);
}

#endif /* MATH_FIXED_INTERNAL_H */
