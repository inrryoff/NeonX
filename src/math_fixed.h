#ifndef MATH_FIXED_H
#define MATH_FIXED_H

#include <stdint.h>
#include <stdbool.h>

#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FLOAT_TO_FIXED(x) ((int32_t)((x) * FIXED_ONE))
#define FIXED_TO_FLOAT(x) ((float)(x) / FIXED_ONE)
#define FIXED_MUL(a, b) (int32_t)(((int64_t)(a) * (b)) >> FIXED_SHIFT)

#define LUT_SIZE 4096
#define RAD_TO_INDEX_FIXED 42722831L
#define FIXED_PI_2 102944

/** Gera um número aleatório seguro de 32 bits. */
uint32_t secure_random_u32(void);

/** Gera uma fase aleatória robusta para animações. */
int32_t neonx_random_phase(void);

/** Inicializa a tabela de consulta trigonométrica. */
void neonx_init_lut(void);

/** Calcula a raiz quadrada inteira. */
uint32_t neonx_isqrt64(uint64_t n);

/** Calcula a distância euclidiana rápida em ponto fixo. */
int32_t neonx_fast_dist_fixed(int32_t dx, int32_t dy);

/** Calcula o seno rápido via interpolação. */
int32_t neonx_fast_sin_fixed(int32_t x_fixed);

#endif

