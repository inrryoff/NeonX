/**
 * NeonX Core Library API
 * High-performance terminal rendering engine with fixed-point math and shaders.
 * 
 * This header consolidates the public interface of the NeonX rendering core.
 * Project: https://github.com/inrryoff/neonx
 */

#ifndef NEONX_PUBLIC_H
#define NEONX_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "math_fixed.h"     /* Fixed-point arithmetic (16.16) */
#include "shader_effects.h" /* Low-level shader function implementations */
#include "shaders.h"        /* Animation presets and management */
#include "render_core.h"    /* Main rendering entry points and color logic */
#include "render_driver.h"  /* Abstract rendering driver definitions */

/**
 * The NeonX library provides:
 * 1. Fixed-point math (16.16): Ensures consistent performance and result across 
 *    different architectures without relying on FPU precision.
 * 2. Real-time shaders: Mathematical effects (Sunset, Matrix, Pulse, etc.) 
 *    designed for character-based terminals.
 * 3. Driver-based rendering: Allows the core to render to CLI (ANSI), WASM, 
 *    or even custom graphical buffers.
 * 4. Animation Presets: Easy-to-use aesthetic profiles for quick integration.
 */

/**
 * Example usage:
 * 
 * RenderDriver driver = { my_set_color, my_reset_color, my_put_char, my_ctx };
 * neonx_render_line(L"Hello NeonX", 11, y_fixed, phase, mode, cx, cy, max_dist, &driver);
 */

#ifdef __cplusplus
}
#endif

#endif /* NEONX_PUBLIC_H */
