#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "shaders.h"

// Mini-framework de testes caseiro
#define ASSERT_NEAR(a, b, epsilon) do { \
    int32_t _a = (a); \
    int32_t _b = (b); \
    int32_t diff = _a > _b ? _a - _b : _b - _a; \
    if (diff > (epsilon)) { \
        fprintf(stderr, "FAIL: %s (%d) near %s (%d) (diff %d > %d) at %s:%d\n", #a, (int)_a, #b, (int)_b, (int)diff, (int)(epsilon), __FILE__, __LINE__); \
        exit(1); \
    } \
} while(0)

int main() {
    printf("--- Iniciando Testes Unitários: NeonX Math ---\n");
    
    // Inicializa tabelas de seno (LUT)
    init_lut();

    // Teste 1: Seno em ponto fixo
    printf("[1/2] Testando fast_sin_fixed...\n");
    ASSERT_NEAR(fast_sin_fixed(0), 0, 500); // Seno(0) = 0
    ASSERT_NEAR(fast_sin_fixed(FIXED_PI_2), FIXED_ONE, 500); // Seno(π/2) = 1
    ASSERT_NEAR(fast_sin_fixed(FLOAT_TO_FIXED(3.14159f)), 0, 500); // Seno(π) = 0
    ASSERT_NEAR(fast_sin_fixed(FLOAT_TO_FIXED(4.71238f)), -FIXED_ONE, 500); // Seno(3π/2) = -1

    // Teste 2: Distância Euclidiana em ponto fixo (Pitágoras)
    printf("[2/2] Testando fast_dist_fixed...\n");
    // Triângulo 3-4-5
    int32_t d3 = FLOAT_TO_FIXED(3.0f);
    int32_t d4 = FLOAT_TO_FIXED(4.0f);
    int32_t d5 = FLOAT_TO_FIXED(5.0f);
    ASSERT_NEAR(fast_dist_fixed(d3, d4), d5, 500);
    ASSERT_NEAR(fast_dist_fixed(0, d5), d5, 500);

    printf("--- TODOS OS TESTES PASSARAM COM SUCESSO ---\n");
    return 0;
}
