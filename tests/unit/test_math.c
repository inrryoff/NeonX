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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "math_fixed.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void test_sin_lut(void) {
    neonx_init_lut();
    int errors = 0;
    for (int deg = 0; deg <= 360; deg++) {
        int32_t fixed_angle = (int32_t)(deg * M_PI / 180.0 * 65536.0);
        int32_t val = neonx_fast_sin_fixed(fixed_angle);
        double expected = sin(deg * M_PI / 180.0);
        int32_t expected_fixed = (int32_t)(expected * 65536.0);
        int diff = abs(val - expected_fixed);
        if (diff > 100) {
            printf("Erro em %d°: got %d (%.5f), expected %d (%.5f)\n", deg, val, (double)val/65536.0, expected_fixed, expected);
            errors++;
        }
    }
    printf(errors ? "LUT seno: %d erros\n" : "LUT seno: OK\n", errors);
}

int main() { 
    test_sin_lut(); 
    return 0; 
}
