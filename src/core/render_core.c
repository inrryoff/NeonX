/*
 * NeonX — Terminal Shader Engine
 * Copyright (C) 2026  inrryoff
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "neonx.h"
#include <wchar.h>
#include <string.h>

#include <stdio.h>

static inline uint32_t compute_delta(void) {
#ifdef __wasm__
    return 0;
#else
    uint32_t a = seed_entropy();
    uint32_t b = alignment();
    uint32_t c = voffset();
    uint32_t d = (uint32_t)NX_RENDER_SEED;
    uint32_t expected = (uint32_t)(a + b + c + d);
    uint32_t actual = nx_fixed(id());

    if ((actual ^ expected) != 0 || !vfs_nodes()) {
#ifdef DEBUG
        fprintf(stderr, "%s", MSG(MSG_DEBUG_ALIGNMENT));
#endif
        return (actual ^ expected) | 0x01;
    }
    return 0;
#endif
}

static int32_t freq_fixed = 19660; 
static int32_t opacity_fixed = 0;
static int32_t gradient_angle_fixed = -65536; 
static bool use_quantization = false;
static bool vertical_opacity_enabled = false;
static bool matte_mode_enabled = false;
static int32_t matte_intensity_fixed = 32768;

void neonx_set_vertical_opacity(bool enabled) { vertical_opacity_enabled = enabled; }
void neonx_set_matte_mode(bool enabled) { matte_mode_enabled = enabled; }
void neonx_set_matte_intensity(int32_t intensity) { matte_intensity_fixed = intensity; }

static int32_t phase_off_r = 0;
static int32_t phase_off_g = 137233;
static int32_t phase_off_b = 274466;

static bool custom_gradient_enabled = false;
static int color1_r = 0, color1_g = 0, color1_b = 0;
static int color2_r = 0, color2_g = 0, color2_b = 0;

static int32_t grad_cos_fixed = 0;
static int32_t grad_sin_fixed = 0;

static void precalc_gradient_angle(void) {
    if (gradient_angle_fixed >= 0) {
        int32_t rad_fixed = (int32_t)(((int64_t)gradient_angle_fixed * DEG_TO_RAD_FIXED) >> FIXED_SHIFT);
        grad_sin_fixed = neonx_fast_sin_fixed(rad_fixed);
        grad_cos_fixed = neonx_fast_sin_fixed(rad_fixed + FIXED_PI_2);
    }
}

void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b) {
    if (op <= 0 || max_dist <= 0) return;
    int32_t dist = neonx_fast_dist_fixed(x - cx, y - cy);
    int32_t ratio = (int32_t)(((int64_t)dist * FIXED_ONE) / max_dist);
    int32_t decay = FIXED_MUL(ratio, op);
    int32_t factor = FIXED_ONE - decay;
    if (factor < 0) factor = 0;
    if (factor > FIXED_ONE) factor = FIXED_ONE;

    *r = (*r * factor) >> FIXED_SHIFT;
    *g = (*g * factor) >> FIXED_SHIFT;
    *b = (*b * factor) >> FIXED_SHIFT;

    if (*r < 0) *r = 0; else if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; else if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; else if (*b > 255) *b = 255;
}
void neonx_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t phase, int *r, int *g, int *b) {
    int32_t p;
    int32_t intensity = FIXED_ONE;

    switch(mode) {
    case 1: p = shader_sunset_fixed(x, y, phase); break;
    case 2: p = phase; break;
    case 3: p = (y >> 1) + phase; break;
    case 4: p = FIXED_MUL(y, 52428) + FIXED_MUL(neonx_fast_sin_fixed(FIXED_MUL(x, 19660)), 196608) + phase; break;
    case 5: p = neonx_fast_dist_fixed(x - cx, y - cy) - phase; break;
    case 6: p = FIXED_MUL(FIXED_MUL(neonx_fast_sin_fixed(FIXED_MUL(x, 13107)), neonx_fast_sin_fixed(FIXED_MUL(y, 13107) + FIXED_PI_2)), 655360) + phase; break;
    case 7: {
        p = phase + FIXED_MUL(x, 6553);
        int32_t diff = x > cx ? x - cx : cx - x;
        int32_t ratio = (cx == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cx);
        intensity = FIXED_ONE - FIXED_MUL(ratio, 52428);
        if(intensity < 13107) intensity = 13107;
        break;
    }
    case 8: {
        p = phase + FIXED_MUL(y, 13107);
        int32_t diff = y > cy ? y - cy : cy - y;
        int32_t ratio = (cy == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cy);
        intensity = FIXED_ONE - FIXED_MUL(ratio, 45875);
        if(intensity < 19660) intensity = 19660;
        break;
    }
    case 9:
        p = phase + FIXED_MUL(x, 6553);
        intensity = ((x >> FIXED_SHIFT) % 2 == 0) ? FIXED_ONE : 39321;
        break;
    case 10: p = shader_matrix_fixed(x, y, phase, &intensity); break;
    case 11: p = shader_pulse_fixed(x, y, cx, cy, phase, &intensity); break;
    case 12: {
        int32_t dist = neonx_fast_dist_fixed(x - cx, y - cy);
        if (dist == 0) dist = 1;
        p = (int32_t)(((int64_t)FIXED_ONE << 14) / dist) - phase;
        int32_t angle = FIXED_MUL(x - cx, 13107) + FIXED_MUL(y - cy, 6553);
        p += angle;
        intensity = FIXED_ONE;
        break;
    }
    case 13: {
        int32_t dx = x > cx ? x - cx : cx - x;
        int32_t dy = y > cy ? y - cy : cy - y;
        p = (dx + dy) - phase;
        intensity = FIXED_ONE;
        break;
    }
    case 14: {
        int32_t h1 = neonx_fast_sin_fixed(FIXED_MUL(x, 13107) + FIXED_MUL(y,  6553) + phase);
        int32_t h2 = neonx_fast_sin_fixed(FIXED_MUL(x, 26214) + FIXED_MUL(y, 13107) + phase * 2);
        int32_t h3 = neonx_fast_sin_fixed(FIXED_MUL(x,  6553) + FIXED_MUL(y, 26214) + phase / 2);
        p = h1 / 2 + h2 / 4 + h3 / 4;
        intensity = FIXED_ONE;
        break;
    }
    case 15: {
        int32_t scan_pos = phase & 0x7FFFFFFF;
        int32_t dist_scan = y - scan_pos;
        if (dist_scan < 0) dist_scan = -dist_scan;
        int32_t width = FIXED_ONE * 8;
        if (dist_scan < width) {
            intensity = FIXED_ONE - FIXED_MUL(dist_scan, FIXED_ONE / 8);
        } else {
            intensity = 32768;
        }
        p = phase + FIXED_MUL(x, 6553);
        break;
    }
    default:
        p = phase + FIXED_MUL(x, 13107) + FIXED_MUL(y, 6553);
        intensity = FIXED_ONE;
        break;
    }

    if (gradient_angle_fixed >= 0) {
        p += FIXED_MUL(grad_cos_fixed, x) + FIXED_MUL(grad_sin_fixed, y);
    }

    int32_t base_phase = FIXED_MUL(freq_fixed, p);
    int raw_r, raw_g, raw_b;

    if (custom_gradient_enabled) {
        int32_t s = neonx_fast_sin_fixed(base_phase);
        int32_t t = (s + FIXED_ONE) >> 1; 

        raw_r = (color1_r * (FIXED_ONE - t) + color2_r * t) >> FIXED_SHIFT;
        raw_g = (color1_g * (FIXED_ONE - t) + color2_g * t) >> FIXED_SHIFT;
        raw_b = (color1_b * (FIXED_ONE - t) + color2_b * t) >> FIXED_SHIFT;
    } else {
        int32_t sin_r = neonx_fast_sin_fixed(base_phase + phase_off_r);
        int32_t sin_g = neonx_fast_sin_fixed(base_phase + phase_off_g);
        int32_t sin_b = neonx_fast_sin_fixed(base_phase + phase_off_b);

        raw_r = ((sin_r * 127) >> FIXED_SHIFT) + 128;
        raw_g = ((sin_g * 127) >> FIXED_SHIFT) + 128;
        raw_b = ((sin_b * 127) >> FIXED_SHIFT) + 128;
    }

    if (mode == 0) {
        *r = raw_r; *g = raw_g; *b = raw_b;
    } else {
        *r = (raw_r * intensity) >> FIXED_SHIFT;
        *g = (raw_g * intensity) >> FIXED_SHIFT;
        *b = (raw_b * intensity) >> FIXED_SHIFT;
    }

    if (use_quantization) {
        *r &= 0xF8; *g &= 0xF8; *b &= 0xF8;
    }

    if (*r < 0) *r = 0; else if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; else if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; else if (*b > 255) *b = 255;
}

void neonx_set_frequency(int32_t freq) {
    freq_fixed = freq;
}

void neonx_set_gradient_angle(int32_t angle) {
    gradient_angle_fixed = angle;
    precalc_gradient_angle();
}

void neonx_set_opacity(int32_t op) {
    opacity_fixed = op;
}

void neonx_set_quantization(bool enable) {
    use_quantization = enable;
}

void neonx_set_palette_offsets(int32_t off_r, int32_t off_g, int32_t off_b) {
    phase_off_r = off_r;
    phase_off_g = off_g;
    phase_off_b = off_b;
}

void neonx_reset_palette(void) {
    phase_off_r = 0;
    phase_off_g = 137233;
    phase_off_b = 274466;
    custom_gradient_enabled = false;
}

void neonx_set_custom_gradient(int r1, int g1, int b1, int r2, int g2, int b2) {
    color1_r = r1; color1_g = g1; color1_b = b1;
    color2_r = r2; color2_g = g2; color2_b = b2;
    custom_gradient_enabled = true;
}

void neonx_render_line(wchar_t *line, size_t line_len, int32_t y_fixed, int32_t phase, int mode, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, RenderDriver *driver) {
    if (!line) return;
    int last_r = -1, last_g = -1, last_b = -1;
    uint32_t delta = compute_delta();
    int noise = (delta != 0) ? 1 : 0;
    int32_t chaos = (int32_t)(delta | (delta >> 13) | (delta << 5));

    for (size_t x = 0; x < line_len; x++) {
        int32_t rx = (int32_t)x << FIXED_SHIFT;
        int32_t ry = y_fixed;

        rx = rx - (noise * (int32_t)((x & 7) << FIXED_SHIFT)) - (chaos & 0x00FF);
        ry = ry - (noise * (int32_t)(((y_fixed >> FIXED_SHIFT) & 3) << FIXED_SHIFT)) + ((chaos >> 8) & 0x00FF);
        
        int32_t cp = phase + (noise * (int32_t)(neonx_fast_sin_fixed((int32_t)(x << 14)) >> 2));

        int r = 0, g = 0, b = 0;
        neonx_get_color(rx, ry, mode, cx_fixed, cy_fixed, cp, &r, &g, &b);

        if (use_quantization) {
            r &= 0xF8; g &= 0xF8; b &= 0xF8;
        }

        if (matte_mode_enabled) {
            int32_t factor = FIXED_ONE - matte_intensity_fixed;
            if (factor < 0) factor = 0;
            r = ((r * factor) >> FIXED_SHIFT) + (10 * (FIXED_ONE - factor) >> FIXED_SHIFT);
            g = ((g * factor) >> FIXED_SHIFT) + (10 * (FIXED_ONE - factor) >> FIXED_SHIFT);
            b = ((b * factor) >> FIXED_SHIFT) + (10 * (FIXED_ONE - factor) >> FIXED_SHIFT);
        } 
        
        if (opacity_fixed > 0) {
            if (vertical_opacity_enabled) {
                int32_t dy = ry > cy_fixed ? ry - cy_fixed : cy_fixed - ry;
                int32_t ratio = (max_dist_fixed > 0) ? (int32_t)(((int64_t)dy * FIXED_ONE) / max_dist_fixed) : 0;
                int32_t decay = FIXED_MUL(ratio * 2, opacity_fixed); 
                int32_t f = FIXED_ONE - decay;
                if (f < 0) f = 0;
                r = (r * f) >> FIXED_SHIFT; g = (g * f) >> FIXED_SHIFT; b = (b * f) >> FIXED_SHIFT;
            } else {
                apply_border_opacity_fixed(rx, 0, cx_fixed, 0, max_dist_fixed, opacity_fixed, &r, &g, &b);
            }
        }

        if (noise) {
            int tr = r, tg = g, tb = b;
            r = ((1 - noise) * r) + (noise * tb);
            g = ((1 - noise) * g) + (noise * tr);
            b = ((1 - noise) * b) + (noise * tg);

            r = (r ^ (noise * 0xFF)) & (0xFF ^ (noise * 0x7F));
            g = (g ^ (noise * 0xFF)) & (0xFF ^ (noise * 0x7F));
            b = (b ^ (noise * 0xFF)) & (0xFF ^ (noise * 0x7F));
        }

        if (line[x] != L' ' && line[x] != L'\t' && line[x] != L'\r' && line[x] != L'\n') {
            if (r != last_r || g != last_g || b != last_b) {
                driver->set_color(driver, r, g, b);
                last_r = r; last_g = g; last_b = b;
            }
        }
        driver->put_char(driver, line[x]);
    }
    driver->reset_color(driver);
}
