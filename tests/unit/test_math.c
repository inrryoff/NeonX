#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "neonx_core.h"
#include "shaders.h"

void test_sin_lut(void) {
    init_lut();
    int errors = 0;
    for (int deg = 0; deg <= 360; deg++) {
        int32_t fixed_angle = (int32_t)(deg * M_PI / 180.0 * 65536.0);
        int32_t val = fast_sin_fixed(fixed_angle);
        double expected = sin(deg * M_PI / 180.0);
        int32_t expected_fixed = (int32_t)(expected * 65536.0);
        int diff = abs(val - expected_fixed);
        if (diff > 100) {
            printf("Erro em %d°: got %d (%.5f), expected %d (%.5f)\n", deg, val, val/65536.0, expected_fixed, expected);
            errors++;
        }
    }
    printf(errors ? "LUT seno: %d erros\n" : "LUT seno: OK\n", errors);
}

int main() { 
    test_sin_lut(); 
    return 0; 
}