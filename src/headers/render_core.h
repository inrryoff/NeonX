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

#ifndef RENDER_CORE_H
#define RENDER_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "neonx.h"

void neonx_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t phase, int *r, int *g, int *b);
void neonx_render_line(wchar_t *line, size_t line_len, int32_t y_fixed, int32_t phase, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, RenderDriver *driver);
void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b);
void neonx_set_frequency(int32_t freq);
void neonx_set_gradient_angle(int32_t angle);
void neonx_set_opacity(int32_t op);
void neonx_set_palette_offsets(int32_t off_r, int32_t off_g, int32_t off_b);
void neonx_reset_palette(void);
void neonx_set_custom_gradient(int r1, int g1, int b1, int r2, int g2, int b2);
void neonx_set_quantization(bool enable);
void neonx_set_vertical_opacity(bool enable);
void neonx_set_matte_mode(bool enabled);
void neonx_set_matte_intensity(int32_t intensity);

#endif


