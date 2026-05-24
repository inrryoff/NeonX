#ifndef SHADERS_H
#define SHADERS_H

#include <stdbool.h>
#include <stdint.h>

#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FLOAT_TO_FIXED(x) ((int32_t)((x) * FIXED_ONE))
#define FIXED_TO_FLOAT(x) ((float)(x) / FIXED_ONE)
#define FIXED_MUL(a, b) (int32_t)(((int64_t)(a) * (b)) / FIXED_ONE)

#define LUT_SIZE 4096
#define PI_2 1.57079632679f

extern float freq;
extern float gradient_angle;
extern float opacity;
extern bool use_quantization;
extern int32_t sin_lut_fixed[LUT_SIZE];

void init_lut(void);
void apply_border_opacity(int x, int y, int len, int count, float op, int *r, int *g, int *b);
void get_color_fast(int x, int y, int mode, int len, int count, float phase, int *r, int *g, int *b);

int32_t fast_sin_fixed(int32_t x_fixed);

#endif
