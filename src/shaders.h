#ifndef SHADERS_H
#define SHADERS_H

#include <stdbool.h>
#include <stdint.h>

#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FLOAT_TO_FIXED(x) ((int32_t)((x) * FIXED_ONE))
#define FIXED_TO_FLOAT(x) ((float)(x) / FIXED_ONE)
#define FIXED_MUL(a, b) (int32_t)(((int64_t)(a) * (b)) >> FIXED_SHIFT)

#define LUT_SIZE 4096
#define FIXED_PI_2 102944

extern int32_t freq_fixed;
extern int32_t gradient_angle_fixed;
extern int32_t opacity_fixed;
extern bool use_quantization;
extern int32_t sin_lut_fixed[LUT_SIZE];
extern int32_t grad_cos_fixed;
extern int32_t grad_sin_fixed;

void init_lut(void);
void precalc_gradient_angle(void);
uint32_t isqrt64(uint64_t n);
int32_t fast_dist_fixed(int32_t dx, int32_t dy);
int32_t fast_sin_fixed(int32_t x_fixed);
void apply_border_opacity_fixed(int32_t x_fixed, int32_t y_fixed, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, int32_t op_fixed, int *r, int *g, int *b);
void get_color_fast(int32_t x_fixed, int32_t y_fixed, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, int32_t phase_fixed, int *r, int *g, int *b);

#endif
