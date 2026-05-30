#ifndef SHADERS_H
#define SHADERS_H

#include "render_core.h"
#include "math_fixed.h"

#define MAX_ANIM_MODE 11

/** Configura o motor de renderização com um perfil estético pré-definido. */
void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed);

#endif

