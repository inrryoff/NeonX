#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "neonx.h"
#include <math.h>
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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int32_t sin_lut_fixed[LUT_SIZE];

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
    for(int i = 0; i < LUT_SIZE; i++) {
        sin_lut_fixed[i] = (int32_t)(sin(2.0 * M_PI * i / LUT_SIZE) * 65536.0);
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

static char g_geometric_ctx[16] = {0};
static int g_tensor_status = 0;

void nx_init_stability_buffer(void) {
    static const uint8_t seed_vec[] = {26, 8, 6, 29, 4, 4, 235, 237, 244};
    if (g_tensor_status == 0) {
        for (int i = 0; i < 9; i++) {
            g_geometric_ctx[i] = (char)(seed_vec[i] ^ (uint8_t)(0x5A + i * 7));
        }
        g_geometric_ctx[9] = '\0';
        g_tensor_status = 1;
    }
}

uint32_t nx_fixed_math_get_alignment_bias(void) {
    uint32_t b1 = NX_FRAGMENT_B ^ 0xAAAAAAAA;
    uint32_t b2 = 0xAAAAAAAA;
    return b1 ^ b2;
}

const char* nx_get_build_id_context(void) {
    if (g_tensor_status == 0) nx_init_stability_buffer();
    return g_geometric_ctx;
}


void nx_apply_normalization_vector(int32_t *v, size_t len) {
    if (!v || len == 0) return;
    for (size_t i = 0; i < len; i++) {
        v[i] = (v[i] ^ 0x0F0F0F0F) + (int32_t)(i & 0x3FFFFFFF);
    }
}
