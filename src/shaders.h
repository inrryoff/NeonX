#ifndef SHADERS_H
#define SHADERS_H

#include "neonx_core.h"

// Modos de animação variam de 0 até 11
#define MAX_ANIM_MODE 11

// Re-exportando macros se necessário (opcional, já que neonx_core.h está incluído)
// Mas manter as assinaturas originais para compatibilidade total

void init_lut(void);
void precalc_gradient_angle(void);
uint32_t isqrt64(uint64_t n);
int32_t fast_dist_fixed(int32_t dx, int32_t dy);
int32_t fast_sin_fixed(int32_t x_fixed);
void apply_border_opacity_fixed(int32_t x_fixed, int32_t y_fixed, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, int32_t op_fixed, int *r, int *g, int *b);
void get_color_fast(int32_t x_fixed, int32_t y_fixed, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, int32_t phase_fixed, int *r, int *g, int *b);

void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed);
void shaders_set_opacity_from_string(const char *o_val);
void shaders_finalize_setup(void);

#endif
