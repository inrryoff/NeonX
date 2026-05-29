#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neonx_core.h"

// Buffer reutilizável para evitar alocações constantes no JS
static char *wasm_buffer = NULL;
static size_t wasm_buffer_size = 0;

/**
 * Inicializa o núcleo NeonX no ambiente WASM.
 */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_init(void) {
    neonx_init_lut();
}

/**
 * Obtém a cor para uma coordenada específica.
 */
EMSCRIPTEN_KEEPALIVE
void neonx_wasm_get_color(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    neonx_get_color(x, y, mode, cx, cy, max_dist, phase, r, g, b);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_frequency(int32_t freq) {
    neonx_set_frequency(freq);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_gradient_angle(int32_t angle) {
    neonx_set_gradient_angle(angle);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_opacity(int32_t op) {
    neonx_set_opacity(op);
}

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_set_quantization(bool enable) {
    neonx_set_quantization(enable);
}

/**
 * Aplica cores a um texto baseado na lógica NeonX.
 * Corrigido para tratar glifos UTF-8 (como █) como uma única unidade de X.
 */
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

    size_t len = strlen(input_text);
    char *out_ptr = wasm_buffer;
    
    // Configurações do grid para o motor de cores (em ponto fixo)
    int grid_x = 0;
    int grid_y = 0;
    int32_t cx = (int32_t)width << (FIXED_SHIFT - 1); // width / 2.0
    int32_t cy = (int32_t)height << (FIXED_SHIFT - 1); // height / 2.0
    
    // Distância máxima do centro até um dos cantos
    int32_t max_dist = neonx_fast_dist_fixed(cx, cy);
    
    int last_r = -1, last_g = -1, last_b = -1;

    for (size_t i = 0; i < len; ) {
        unsigned char c = (unsigned char)input_text[i];
        
        // Tratamento de quebra de linha
        if (c == '\n') {
            grid_y++;
            grid_x = 0;
            *out_ptr++ = (char)c;
            i++;
            continue;
        }

        // Determina quantos bytes tem o caractere UTF-8 atual
        int char_len = 1;
        if (c >= 0xF0) char_len = 4;
        else if (c >= 0xE0) char_len = 3;
        else if (c >= 0xC0) char_len = 2;

        if (i + (size_t)char_len > len) char_len = 1;

        // Espaços e tabs incrementam X mas não calculam cor
        if (c == ' ' || c == '\t' || c == '\r') {
            for(int j=0; j<char_len; j++) *out_ptr++ = input_text[i+j];
            grid_x++;
            i += (size_t)char_len;
            continue;
        }

        // Calcula cor para o glifo atual (X, Y)
        int r, g, b;
        neonx_get_color((int32_t)grid_x << FIXED_SHIFT, (int32_t)grid_y << FIXED_SHIFT, mode, cx, cy, max_dist, phase, &r, &g, &b);

        // Injeta ANSI se a cor mudou
        if (r != last_r || g != last_g || b != last_b) {
            out_ptr += sprintf(out_ptr, "\033[38;2;%d;%d;%dm", r, g, b);
            last_r = r; last_g = g; last_b = b;
        }
        
        // Copia todos os bytes do glifo
        for (int j = 0; j < char_len; j++) {
            *out_ptr++ = input_text[i + j];
        }
        
        grid_x++;
        i += (size_t)char_len;
    }
    
    // Reset final e terminação
    strcpy(out_ptr, "\033[0m");
    out_ptr += 4;
    *out_ptr = '\0';

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
            neonx_get_color((int32_t)x << FIXED_SHIFT, y_fixed, mode, cx, cy, max_dist, phase, &r, &g, &b);
            
            int idx = (y * width + x) * 4;
            buffer[idx] = (uint8_t)r;
            buffer[idx+1] = (uint8_t)g;
            buffer[idx+2] = (uint8_t)b;
            buffer[idx+3] = 255; 
        }
    }
}
