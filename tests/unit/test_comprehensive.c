#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "math_fixed.h"
#include "render_core.h"
#include "shader_effects.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define FLOAT_TO_FIXED(f) ((int32_t)((f) * 65536.0))
#define FIXED_TO_FLOAT(x) ((double)(x) / 65536.0)

#define CLR_R "\033[0;31m"
#define CLR_G "\033[0;32m"
#define CLR_Y "\033[1;33m"
#define CLR_B "\033[0;34m"
#define CLR_N "\033[0m"

#ifdef VERBOSE_DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while (0)
#endif

int total_errors = 0;

void assert_fixed_near(const char* label, int32_t got, int32_t expected, int32_t tolerance, const char* context) {
    int32_t diff = abs(got - expected);
    if (diff > tolerance) {
        printf(CLR_R "FAIL: %s" CLR_N " | Got: %d (%.4f) | Expected: %d (%.4f) | Diff: %d | Context: %s\n", 
               label, got, FIXED_TO_FLOAT(got), expected, FIXED_TO_FLOAT(expected), diff, context);
        total_errors++;
    } else {
        DEBUG_PRINT(CLR_G "PASS: %s" CLR_N " | Got: %d | Context: %s\n", label, got, context);
    }
}

void assert_int_equal(const char* label, int got, int expected, const char* context) {
    if (got != expected) {
        printf(CLR_R "FAIL: %s" CLR_N " | Got: %d | Expected: %d | Context: %s\n", 
               label, got, expected, context);
        total_errors++;
    } else {
        DEBUG_PRINT(CLR_G "PASS: %s" CLR_N " | Got: %d | Context: %s\n", label, got, context);
    }
}

// 1. Testes de Matemática (Ponto Fixo e Overflow)
void test_math_comprehensive() {
    printf(CLR_B "=== Testando Matemática (Ponto Fixo & Precisão) ===\n" CLR_N);
    
    // Seno Precision
    neonx_init_lut();
    for (int deg = 0; deg <= 360; deg += 45) {
        double rad = deg * M_PI / 180.0;
        int32_t angle = FLOAT_TO_FIXED(rad);
        int32_t got = neonx_fast_sin_fixed(angle);
        int32_t expected = FLOAT_TO_FIXED(sin(rad));
        char ctx[64]; sprintf(ctx, "sin(%d°)", deg);
        assert_fixed_near("Math Sin", got, expected, 200, ctx);
    }

    // SQRT & Dist (Overflow Check)
    // Coordenadas que geram d^2 > 2^32, mas o resultado cabe em int32
    int32_t large_val = 20000 << FIXED_SHIFT; // 20000.0
    int32_t dist = neonx_fast_dist_fixed(large_val, large_val);
    // dist = sqrt(20000^2 + 20000^2) = 28284.27
    int32_t expected_dist = FLOAT_TO_FIXED(28284.2712);
    assert_fixed_near("Math Dist Overflow", dist, expected_dist, 100, "Large coordinates (20k, 20k)");

    // SQRT Pequena
    dist = neonx_fast_dist_fixed(10, 10);
    assert_fixed_near("Math Dist Small", dist, 14, 1, "Small coordinates (10, 10 units)");
}

// 2. Testes de Opacidade e Cores
void test_render_logic() {
    printf(CLR_B "=== Testando Lógica de Renderização & Cores ===\n" CLR_N);

    int32_t cx = 100 << FIXED_SHIFT;
    int32_t cy = 100 << FIXED_SHIFT;
    int32_t max_dist = 200 << FIXED_SHIFT;
    int32_t opacity = FIXED_ONE; // 1.0

    // Caso: No centro (Opacidade não deve afetar)
    int r = 255, g = 100, b = 50;
    apply_border_opacity_fixed(cx, cy, cx, cy, max_dist, opacity, &r, &g, &b);
    assert_int_equal("Color Center", r, 255, "Red at center");
    assert_int_equal("Color Center", g, 100, "Green at center");

    // Caso: Na borda exata (Opacidade 1.0 deve zerar)
    r = 255; g = 255; b = 255;
    apply_border_opacity_fixed(cx + max_dist, cy, cx, cy, max_dist, opacity, &r, &g, &b);
    assert_int_equal("Color Edge (Max Op)", r, 0, "Red at edge max op");

    // Caso: Meio do caminho, opacidade 0.5
    r = 200; g = 200; b = 200;
    // dist = 100, max_dist = 200 -> ratio = 0.5. decay = 0.5 * 0.5 = 0.25. factor = 0.75.
    // 200 * 0.75 = 150.
    apply_border_opacity_fixed(cx + (100 << FIXED_SHIFT), cy, cx, cy, max_dist, FIXED_ONE / 2, &r, &g, &b);
    assert_int_equal("Color Mid (Half Op)", r, 150, "Red at mid dist half op");
}

// 3. Testes de Efeitos (Shaders)
void test_shaders() {
    printf(CLR_B "=== Testando Shaders (Efeitos Visuais) ===\n" CLR_N);
    
    int32_t intensity = 0;
    int32_t phase = 0;
    
    // Pulse shader logic
    shader_pulse_fixed(100 << FIXED_SHIFT, 100 << FIXED_SHIFT, 100 << FIXED_SHIFT, 100 << FIXED_SHIFT, phase, &intensity);
    // No centro, dist=0. sin(0)=0. intensity = (0+1)/2 = 0.5. scaled = 0.5 * 0.8 + 0.2 = 0.6.
    assert_fixed_near("Pulse Intensity Center", intensity, FLOAT_TO_FIXED(0.6), 500, "Pulse at center");
}

int main(int argc, char** argv) {
    printf("--- Iniciando Suíte de Testes NeonX ---\n");
    
    test_math_comprehensive();
    test_render_logic();
    test_shaders();
    
    if (total_errors > 0) {
        printf(CLR_R "\nRelatório: %d erros detectados.\n" CLR_N, total_errors);
        return 1;
    } else {
        printf(CLR_G "\nRelatório: Todos os testes passaram com sucesso!\n" CLR_N);
        return 0;
    }
}
