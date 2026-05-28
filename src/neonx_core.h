#ifndef NEONX_CORE_H
#define NEONX_CORE_H

#include <stdbool.h>
#include <stdint.h>

// --- MACROS PARA MATEMÁTICA EM PONTO FIXO (FIXED-POINT) ---
#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FLOAT_TO_FIXED(x) ((int32_t)((x) * FIXED_ONE))
#define FIXED_TO_FLOAT(x) ((float)(x) / FIXED_ONE)
#define FIXED_MUL(a, b) (int32_t)(((int64_t)(a) * (b)) >> FIXED_SHIFT)

// Valor de PI/2 em escala Fixed-Point (aprox 1.570796 * 65536)
#define FIXED_PI_2 102944

// --- DECLARAÇÕES DE FUNÇÕES DO NÚCLEO ---

/**
 * Inicializa a tabela de lookup (LUT) para funções trigonométricas.
 */
void neonx_init_lut(void);

/**
 * Calcula o seno rápido em ponto fixo usando a LUT.
 */
int32_t neonx_fast_sin_fixed(int32_t x);

/**
 * Calcula a raiz quadrada rápida para 64 bits.
 */
uint32_t neonx_isqrt64(uint64_t n);

/**
 * Calcula a distância 2D rápida em ponto fixo.
 */
int32_t neonx_fast_dist_fixed(int32_t dx, int32_t dy);

/**
 * Função principal para obter a cor (RGB) de um pixel/caractere.
 * @param x, y Coordenadas do pixel.
 * @param mode Modo de animação (shader).
 * @param cx, cy Centro da animação.
 * @param max_dist Distância máxima para efeitos de borda.
 * @param phase Fase atual da animação (tempo).
 * @param r, g, b Ponteiros para os componentes de cor resultantes (0-255).
 */
void neonx_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b);

// --- SETTERS DE CONFIGURAÇÃO ---

void neonx_set_frequency(int32_t freq);
void neonx_set_gradient_angle(int32_t angle);
void neonx_set_opacity(int32_t op);
void neonx_set_quantization(bool enable);

#endif // NEONX_CORE_H
