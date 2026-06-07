/*
 * NeonX — Terminal Shader Engine
 * Copyright (C) 2026  inrryoff
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
