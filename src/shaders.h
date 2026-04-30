#ifndef SHADERS_H
#define SHADERS_H

#include <stdbool.h>

extern double freq;
extern double diagonal;
extern double opacity;

void init_lut(void);
void get_color_fast(int x, int y, int mode, int len, int count, double phase, int *r, int *g, int *b);

#endif
