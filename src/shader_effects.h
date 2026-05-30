#ifndef SHADER_EFFECTS_H
#define SHADER_EFFECTS_H

#include <stdint.h>

/** Implementa o efeito visual de pôr-do-sol. */
int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase);

/** Gera um valor aleatório baseado em coordenadas e fase. */
int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase);

/** Implementa o efeito visual inspirado no filme Matrix. */
int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity);

/** Implementa o efeito de pulsação radial sincronizada. */
int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity);

#endif

