/**
 * NeonX Core Library API - Unified Parent Header
 * High-performance terminal rendering engine with fixed-point math and shaders.
 * 
 * This header consolidates the public interface of the NeonX rendering core.
 */

#ifndef NEONX_H
#define NEONX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "build_config.h"
#include "msgs.h"
#include "math_fixed.h"
#include "math_fixed_internal.h"
#include "style.h"
#include "monocypher.h"
#include "integrity.h"
#include "render_driver.h"
#include "render_core.h"
#include "shader_effects.h"
#include "terminal.h"
#include "render.h"
#include "shaders.h"

#ifdef __cplusplus
}
#endif

#endif /* NEONX_H */
