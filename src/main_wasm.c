#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "neonx.h"

typedef struct {
    char *ptr;
    size_t rem;
    int last_r, last_g, last_b;
} WasmDriverCtx;

static void wasm_set_color(RenderDriver *self, int r, int g, int b) {
    WasmDriverCtx *ctx = (WasmDriverCtx*)self->ctx;
    if (r == ctx->last_r && g == ctx->last_g && b == ctx->last_b) return;
    int n = snprintf(ctx->ptr, ctx->rem, "\033[38;2;%d;%d;%dm", r, g, b);
    if (n > 0) { ctx->ptr += n; ctx->rem -= (size_t)n; }
    ctx->last_r = r; ctx->last_g = g; ctx->last_b = b;
}

static void wasm_reset_color(RenderDriver *self) {
    WasmDriverCtx *ctx = (WasmDriverCtx*)self->ctx;
    int n = snprintf(ctx->ptr, ctx->rem, "\033[0m");
    if (n > 0) { ctx->ptr += n; ctx->rem -= (size_t)n; }
}

static void wasm_put_char(RenderDriver *self, wchar_t c) {
    WasmDriverCtx *ctx = (WasmDriverCtx*)self->ctx;
    char mb[8];
    int n = wctomb(mb, c);
    if (n <= 0) { mb[0] = (char)c; n = 1; }
    if ((size_t)n < ctx->rem) {
        memcpy(ctx->ptr, mb, (size_t)n);
        ctx->ptr += n;
        ctx->rem -= (size_t)n;
    }
}

static char *wasm_buffer = NULL;
static size_t wasm_buffer_size = 0;

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_init(void) {
    setlocale(LC_ALL, "C.UTF-8");
    neonx_init_lut();
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_vertical_opacity(int enabled) {
    neonx_set_vertical_opacity(enabled ? 1 : 0);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_matte_mode(int enabled) {
    neonx_set_matte_mode(enabled ? 1 : 0);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_matte_intensity(int32_t intensity) {
    neonx_set_matte_intensity(intensity);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t phase, int *r, int *g, int *b) {
    neonx_get_color(x, y, mode, cx, cy, phase, r, g, b);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_frequency(int32_t freq) { neonx_set_frequency(freq); }

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_gradient_angle(int32_t angle) { neonx_set_gradient_angle(angle); }

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_opacity(int32_t op) { neonx_set_opacity(op); }

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_quantization(bool enable) { neonx_set_quantization(enable); }

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_custom_gradient(int r1, int g1, int b1, int r2, int g2, int b2) {
    neonx_set_custom_gradient(r1, g1, b1, r2, g2, b2);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_reset_palette(void) { neonx_reset_palette(); }

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_palette_offsets(int32_t r, int32_t g, int32_t b) {
    neonx_set_palette_offsets(r, g, b);
}

EMSCRIPTEN_KEEPALIVE
bool neonx_wasm_set_preset(const char* name, int* out_mode, int32_t* out_speed) {
    struct neonx_options opts = {0};
    if (shaders_set_preset(name, &opts)) {
        *out_mode = opts.anim_mode;
        *out_speed = opts.speed_fixed;
        neonx_set_frequency(opts.freq_fixed);
        neonx_set_gradient_angle(opts.angle_fixed);
        return true;
    }
    return false;
}

EMSCRIPTEN_KEEPALIVE
char* neonx_apply_colors(const char* input_text, int mode, int width, int height, int32_t phase) {
    if (!input_text) return NULL;
    
    size_t needed = strlen(input_text) * 48 + 1024;
    if (needed > wasm_buffer_size) {
        char *newbuf = (char *)realloc(wasm_buffer, needed);
        if (!newbuf) return NULL;
        wasm_buffer = newbuf;
        wasm_buffer_size = needed;
    }

    WasmDriverCtx ctx = {wasm_buffer, wasm_buffer_size, -1, -1, -1};
    RenderDriver driver = {wasm_set_color, wasm_reset_color, wasm_put_char, &ctx};
    
    int32_t cx = (int32_t)width << (FIXED_SHIFT - 1);
    int32_t cy = (int32_t)height << (FIXED_SHIFT - 1);
    int32_t max_dist = neonx_fast_dist_fixed(cx, cy);
    
    size_t wlen = mbstowcs(NULL, input_text, 0);
    if (wlen == (size_t)-1) return NULL;
    wchar_t *winput = malloc((wlen + 1) * sizeof(wchar_t));
    if (!winput) return NULL;
    mbstowcs(winput, input_text, wlen + 1);

    int grid_y = 0;
    wchar_t *line_start = winput;
    for (size_t i = 0; i <= wlen; i++) {
        if (winput[i] == L'\n' || winput[i] == L'\0') {
            wchar_t saved = winput[i];
            winput[i] = L'\0';
            size_t line_len = (size_t)(&winput[i] - line_start);
            neonx_render_line(line_start, line_len, (int32_t)grid_y << FIXED_SHIFT, phase, mode, cx, cy, max_dist, &driver);
            if (saved == L'\n') {
                wasm_put_char(&driver, L'\n');
                grid_y++;
                line_start = &winput[i+1];
            }
        }
    }
    
    free(winput);
    if (ctx.rem > 0) *ctx.ptr = '\0';
    return wasm_buffer;
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_render_canvas(uint8_t* buffer, int width, int height, int mode, int32_t phase) {
    int32_t cx = (int32_t)width << (FIXED_SHIFT - 1);
    int32_t cy = (int32_t)height << (FIXED_SHIFT - 1);
    int32_t max_dist = neonx_fast_dist_fixed(cx, cy);

    for (int y = 0; y < height; y++) {
        int32_t y_fixed = (int32_t)y << FIXED_SHIFT;
        for (int x = 0; x < width; x++) {
            int r, g, b;
            neonx_get_color((int32_t)x << FIXED_SHIFT, y_fixed, mode, cx, cy, phase, &r, &g, &b);
            
            int idx = (y * width + x) * 4;
            buffer[idx] = (uint8_t)r;
            buffer[idx+1] = (uint8_t)g;
            buffer[idx+2] = (uint8_t)b;
            buffer[idx+3] = 255; 
        }
    }
}

