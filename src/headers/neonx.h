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

#ifndef NEONX_H
#define NEONX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "build_config.h"
#include "msgs.h"
#include "math_fixed.h"
#include "math_fixed_internal.h"
#include "style.h"
#include "monocypher.h"
#include "integrity.h"
#include "render_driver.h"
#include "render_core.h"
#include "shader_effects.h"
#include "terminal.h"
#include "render.h"
#include "shaders.h"

#ifdef __cplusplus
}
#endif

#endif
