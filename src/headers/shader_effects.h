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

#ifndef SHADER_EFFECTS_H
#define SHADER_EFFECTS_H

#include <stdint.h>

int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase);
int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase);
int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity);
int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity);

#endif

