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
