#ifndef SHADER_EFFECTS_H
#define SHADER_EFFECTS_H

#include <stdint.h>

int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase);
int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase);
int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity);
int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity);

#endif

