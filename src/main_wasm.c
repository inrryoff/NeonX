#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "math_fixed.h"
#include "render_core.h"
#include "render_driver.h"

static char *wasm_buffer = NULL;
static size_t wasm_buffer_size = 0;

typedef struct {
    char *ptr;
    size_t rem;
    int last_r, last_g, last_b;
} WasmDriverCtx;

/** Define a cor ANSI 24-bits para o ambiente WebAssembly. */
static void wasm_set_color(RenderDriver *self, int r, int g, int b) {
    WasmDriverCtx *ctx = (WasmDriverCtx*)self->ctx;
    if (r == ctx->last_r && g == ctx->last_g && b == ctx->last_b) return;
    int n = snprintf(ctx->ptr, ctx->rem, "\033[38;2;%d;%d;%dm", r, g, b);
    if (n > 0) { ctx->ptr += n; ctx->rem -= (size_t)n; }
    ctx->last_r = r; ctx->last_g = g; ctx->last_b = b;
}

/** Reseta os atributos de cor no buffer WASM. */
static void wasm_reset_color(RenderDriver *self) {
    WasmDriverCtx *ctx = (WasmDriverCtx*)self->ctx;
    int n = snprintf(ctx->ptr, ctx->rem, "\033[0m");
    if (n > 0) { ctx->ptr += n; ctx->rem -= (size_t)n; }
}

/** Converte e anexa um caractere largo ao buffer de saída WASM. */
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

/** Inicializa as tabelas matemáticas para o ambiente WebAssembly. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_init(void) {
    neonx_init_lut();
}

/** Exporta a função de cálculo de cor para interface JavaScript. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    neonx_get_color(x, y, mode, cx, cy, max_dist, phase, r, g, b);
}

/** Ajusta a frequência de oscilação via interface WASM. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_frequency(int32_t freq) {
    neonx_set_frequency(freq);
}

/** Define o ângulo do gradiente via interface WASM. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_gradient_angle(int32_t angle) {
    neonx_set_gradient_angle(angle);
}

/** Ajusta a opacidade das bordas via interface WASM. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_opacity(int32_t op) {
    neonx_set_opacity(op);
}

/** Alterna o modo de quantização de cores via interface WASM. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_quantization(bool enable) {
    neonx_set_quantization(enable);
}

/** Aplica os efeitos de cor NeonX a uma string de texto no ambiente Web. */
EMSCRIPTEN_KEEPALIVE
char* neonx_apply_colors(const char* input_text, int mode, int width, int height, int32_t phase) {
    if (!input_text) return NULL;
    
    size_t needed = strlen(input_text) * 36 + 512;
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
            neonx_render_line(line_start, (int32_t)grid_y << FIXED_SHIFT, phase, mode, cx, cy, max_dist, &driver);
            if (saved == L'\n') {
                wasm_put_char(&driver, L'\n');
                grid_y++;
                line_start = &winput[i+1];
            }
        }
    }
    
    free(winput);
    *ctx.ptr = '\0';
    return wasm_buffer;
}

/** Renderiza diretamente em um buffer de pixels (Canvas) via WebAssembly. */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_render_canvas(uint8_t* buffer, int width, int height, int mode, int32_t phase) {
    int32_t cx = (int32_t)width << (FIXED_SHIFT - 1);
    int32_t cy = (int32_t)height << (FIXED_SHIFT - 1);
    int32_t max_dist = neonx_fast_dist_fixed(cx, cy);

    for (int y = 0; y < height; y++) {
        int32_t y_fixed = (int32_t)y << FIXED_SHIFT;
        for (int x = 0; x < width; x++) {
            int r, g, b;
            neonx_get_color((int32_t)x << FIXED_SHIFT, y_fixed, mode, cx, cy, max_dist, phase, &r, &g, &b);
            
            int idx = (y * width + x) * 4;
            buffer[idx] = (uint8_t)r;
            buffer[idx+1] = (uint8_t)g;
            buffer[idx+2] = (uint8_t)b;
            buffer[idx+3] = 255; 
        }
    }
}

