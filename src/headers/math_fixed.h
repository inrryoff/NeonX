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

#ifndef MATH_FIXED_H
#define MATH_FIXED_H

#include <stdint.h>
#include <stdbool.h>

#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FIXED_MUL(a, b) (int32_t)(((int64_t)(a) * (b)) >> FIXED_SHIFT)

#define LUT_SIZE 4096
#define RAD_TO_INDEX_FIXED 42722831L
#define DEG_TO_RAD_FIXED  1143
#define FIXED_PI_2 102944

uint32_t secure_random_u32(void);
int32_t neonx_random_phase(void);
void neonx_init_lut(void);
uint32_t neonx_isqrt64(uint64_t n);
int32_t neonx_fast_dist_fixed(int32_t dx, int32_t dy);
int32_t neonx_fast_sin_fixed(int32_t x_fixed);
const char*  id(void);
uint32_t     alignment(void);

#endif

