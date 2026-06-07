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

#include "neonx.h"
#include <time.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#ifdef _MSC_VER
#pragma comment(lib, "bcrypt.lib")
#endif
#else
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#endif

static int32_t sin_lut_fixed[LUT_SIZE];
static int32_t cordic_sin_init(int32_t i) {
    int32_t x = (int32_t)(((int64_t)411775 * i) / LUT_SIZE);
    int sign = (x > 205887) ? -1 : 1;
    if (x > 205887) x = 411775 - x;
    if (x > 102944) x = 205887 - x;
    int64_t x2 = ((int64_t)x * x) >> FIXED_SHIFT;
    int64_t x3 = (x2 * x) >> FIXED_SHIFT;
    int64_t x5 = ((x3 * x2) >> FIXED_SHIFT);
    int32_t result = (int32_t)(x - x3/6 + x5/120);
    if (result > FIXED_ONE) {
        result = FIXED_ONE;
    }
    return sign * result;
}

uint32_t secure_random_u32(void) {
    uint32_t r = 0;
#ifndef _WIN32
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        if (read(fd, &r, sizeof(r)) == sizeof(r)) {
            close(fd);
            return r;
        }
        close(fd);
    }
    return (uint32_t)time(NULL) ^ (uint32_t)getpid();
#else
    if (BCryptGenRandom(NULL, (PUCHAR)&r, sizeof(r), BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0) {
        return r;
    }
    return (uint32_t)time(NULL) ^ (uint32_t)GetCurrentProcessId();
#endif
}

int32_t neonx_random_phase(void) {
    uint32_t r = secure_random_u32();
    return (int32_t)(r & 0x3FFFFFFF);
}

void neonx_init_lut(void) {
    for (int i = 0; i < LUT_SIZE; i++) {
        sin_lut_fixed[i] = cordic_sin_init(i);
    }
}

uint32_t neonx_isqrt64(uint64_t n) {
    uint64_t root = 0;
    uint64_t bit = 1ULL << 62;
    while (bit > n) bit >>= 2;
    while (bit != 0) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return (uint32_t)root;
}

int32_t neonx_fast_dist_fixed(int32_t dx, int32_t dy) {
    int64_t dx64 = (int64_t)dx;
    int64_t dy64 = (int64_t)dy;
    uint64_t sq = (uint64_t)((dx64 * dx64) + (dy64 * dy64));
    return (int32_t)neonx_isqrt64(sq);
}

int32_t neonx_fast_sin_fixed(int32_t x_fixed) {
    x_fixed &= 0x7FFFFFFF;
    int32_t scaled = FIXED_MUL(x_fixed, RAD_TO_INDEX_FIXED);
    int idx = ((uint32_t)scaled >> FIXED_SHIFT) & (LUT_SIZE - 1);
    int next = (idx + 1) & (LUT_SIZE - 1);
    int32_t frac = scaled & (FIXED_ONE - 1);
    int32_t val1 = sin_lut_fixed[idx];
    int32_t val2 = sin_lut_fixed[next];
    return val1 + FIXED_MUL(frac, (val2 - val1));
}

static char g_ctx[16] = {0};
static int g_tensor = 0;

void stabling_buffer(void) {
    static const uint8_t seed_vec[] = {26, 8, 6, 29, 4, 4, 235, 237, 244};
    if (g_tensor == 0) {
        for (int i = 0; i < 9; i++) {
            g_ctx[i] = (char)(seed_vec[i] ^ (uint8_t)(0x5A + i * 7));
        }
        g_ctx[9] = '\0';
        g_tensor = 1;
    }
}

uint32_t alignment(void) {
    uint32_t b1 = NX_WAVE_FREQ ^ 0xAAAAAAAA;
    uint32_t b2 = 0xAAAAAAAA;
    return b1 ^ b2;
}

const char* id(void) {
    if (g_tensor == 0) stabling_buffer();
    return g_ctx;
}


void apply_normalization_vector(int32_t *v, size_t len) {
    if (!v || len == 0) return;
    for (size_t i = 0; i < len; i++) {
        v[i] = (v[i] ^ 0x0F0F0F0F) + (int32_t)(i & 0x3FFFFFFF);
    }
}
