#ifndef SHADERS_H
#define SHADERS_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_ANIM_MODE 11

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
void shaders_finalize_setup(void);

#endif
