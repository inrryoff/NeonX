// ==================== shaders.h ====================
#ifndef SHADERS_H
#define SHADERS_H

#include <stdbool.h>
#include <stdint.h>

// Modos de animação variam de 0 até 11
#define MAX_ANIM_MODE 11

// --- MACROS PARA MATEMÁTICA EM PONTO FIXO (FIXED-POINT) ---
// Números "fixed-point" resolvem o problema da lentidão de operações de ponto 
// flutuante (float) em alguns processadores. A ideia é tratar um número muito 
// grande inteiro como se tivesse uma "vírgula invisível". 
// Usaremos 16 casas binárias (bits) para representar a parte decimal.

#define FIXED_SHIFT 16
// FIXED_ONE representa o valor inteiro '1.0' (equivale a 65536)
#define FIXED_ONE (1 << FIXED_SHIFT)

// FLOAT_TO_FIXED: Pega um valor float (ex: 0.5) e o multiplica por 65536 para 
// torná-lo um inteiro fixed-point.
#define FLOAT_TO_FIXED(x) ((int32_t)((x) * FIXED_ONE))

// FIXED_TO_FLOAT: O inverso da de cima, devolve o ponto flutuante dividindo por 65536.
#define FIXED_TO_FLOAT(x) ((float)(x) / FIXED_ONE)

// FIXED_MUL: Multiplica dois valores fixed-point. Como multiplicar dois inteiros grandes
// resulta num número gigante, nós fazemos um shift-right (>> 16) para compensar a escala e
// voltar ao normal. Usa int64_t temporariamente para prevenir overflow durante a conta.
#define FIXED_MUL(a, b) (int32_t)(((int64_t)(a) * (b)) >> FIXED_SHIFT)

// O tamanho da tabela LUT (Look Up Table). Teremos 4096 divisões em uma onda senoidal
#define LUT_SIZE 4096

// Valor de PI/2 em escala Fixed-Point (aprox 1.570796 * 65536)
#define FIXED_PI_2 102944

// Assinaturas das funções para uso externo
void init_lut(void);
void precalc_gradient_angle(void);
uint32_t isqrt64(uint64_t n);
int32_t fast_dist_fixed(int32_t dx, int32_t dy);
int32_t fast_sin_fixed(int32_t x_fixed);
void apply_border_opacity_fixed(int32_t x_fixed, int32_t y_fixed, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, int32_t op_fixed, int *r, int *g, int *b);
void get_color_fast(int32_t x_fixed, int32_t y_fixed, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, int32_t phase_fixed, int *r, int *g, int *b);
void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed);
void shaders_set_frequency(int32_t freq);
void shaders_set_gradient_angle(int32_t angle);
void shaders_set_opacity_from_string(const char *o_val);
void shaders_set_quantization(bool enable); 
void shaders_finalize_setup(void);

#endif
