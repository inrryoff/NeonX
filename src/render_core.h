#ifndef RENDER_CORE_H
#define RENDER_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "render_driver.h"

/** Obtém os componentes RGB para uma coordenada e modo. */
void neonx_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b);

/** Renderiza uma linha de caracteres utilizando um driver. */
void neonx_render_line(wchar_t *line, int32_t y_fixed, int32_t phase, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, RenderDriver *driver);

/** Aplica o efeito de opacidade nas bordas. */
void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b);

/** Define a frequência global das ondas. */
void neonx_set_frequency(int32_t freq);

/** Define o ângulo de inclinação do gradiente. */
void neonx_set_gradient_angle(int32_t angle);

/** Define o nível de opacidade das bordas. */
void neonx_set_opacity(int32_t op);

/** Ativa ou desativa a redução de cores (quantização). */
void neonx_set_quantization(bool enable);

#endif

