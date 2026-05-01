#ifndef SHADERS_H
#define SHADERS_H

#include <stdbool.h>

#define LUT_SIZE 4096
#define PI_2 1.57079632679

extern double freq;
extern double diagonal;
extern double opacity;
extern double sin_lut[LUT_SIZE];

void init_lut(void);
void apply_border_opacity(int x, int y, int len, int count, double op, int *r, int *g, int *b);
void get_color_fast(int x, int y, int mode, int len, int count, double phase, int *r, int *g, int *b);

#endif
