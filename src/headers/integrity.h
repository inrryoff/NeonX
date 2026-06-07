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

#ifndef INTEGRITY_H
#define INTEGRITY_H

#include <stdbool.h>
#include <stdint.h>

static const unsigned char NEONX_OFFICIAL_PUBLIC_KEY[32] = {
    0x53, 0x0F, 0xCC, 0xD0, 0xB1, 0x41, 0x65, 0x9D,
    0x15, 0xA7, 0x59, 0xBE, 0x4D, 0xAC, 0x2D, 0xE4,
    0x4C, 0xB0, 0x32, 0x21, 0x45, 0xF6, 0x1D, 0x71,
    0x06, 0xF6, 0xE5, 0xF1, 0x71, 0xE0, 0x48, 0x9D
};

int   check_integrity(void);
bool  is_using_official_key(void);
uint32_t seed_entropy(void);
bool  vfs_nodes(void);

#endif
