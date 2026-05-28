#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "neonx_core.h"

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
char* neonx_apply_colors(const char* input_text, int mode, int width, int height, float phase_f) {
    if (!input_text) return NULL;

    size_t len = strlen(input_text);
    // Reservamos espaço suficiente para os escapes ANSI
    size_t out_capacity = len * 32 + 1024; 
    char* output = (char*)malloc(out_capacity);
    if (!output) return NULL;

    char* out_ptr = output;
    int32_t phase = FLOAT_TO_FIXED(phase_f);
    
    int32_t cx = FLOAT_TO_FIXED(width / 2.0f);
    int32_t cy = FLOAT_TO_FIXED(height / 2.0f);
    float diag = sqrtf((float)width * width + (float)height * height);
    int32_t max_dist = FLOAT_TO_FIXED(diag / 2.0f);

    int x = 0;
    int y = 0;
    int last_r = -1, last_g = -1, last_b = -1;

    for (size_t i = 0; i < len; ) {
        unsigned char c = (unsigned char)input_text[i];

        // 1. Tratamento de nova linha
        if (c == '\n') {
            *out_ptr++ = '\n';
            y++;
            x = 0;
            i++;
            continue;
        }

        // 2. Determina quantos bytes tem o caractere UTF-8 atual
        int char_len = 1;
        if (c >= 0xF0) char_len = 4;
        else if (c >= 0xE0) char_len = 3;
        else if (c >= 0xC0) char_len = 2;

        // Segurança contra strings malformadas
        if (i + char_len > len) char_len = 1;

        // 3. Espaços e tabs incrementam X mas não calculam cor (mantêm transparência/fundo)
        if (c == ' ' || c == '\t' || c == '\r') {
            for(int j=0; j<char_len; j++) *out_ptr++ = input_text[i+j];
            x++;
            i += char_len;
            continue;
        }

        // 4. Calcula cor para o glifo atual (X, Y)
        int r, g, b;
        neonx_get_color(FLOAT_TO_FIXED(x), FLOAT_TO_FIXED(y), mode, cx, cy, max_dist, phase, &r, &g, &b);

        // 5. Injeta ANSI se a cor mudou
        if (r != last_r || g != last_g || b != last_b) {
            out_ptr += sprintf(out_ptr, "\033[38;2;%d;%d;%dm", r, g, b);
            last_r = r; last_g = g; last_b = b;
        }
        
        // 6. Copia todos os bytes do glifo (ex: os 3 bytes de '█')
        for (int j = 0; j < char_len; j++) {
            *out_ptr++ = input_text[i + j];
        }
        
        // 7. Incrementa X e pula os bytes lidos
        x++;
        i += char_len;
    }
    
    // Reset final e terminação
    out_ptr += sprintf(out_ptr, "\033[0m");
    *out_ptr = '\0';

    return output;
}

// Importação necessária (já deve estar incluída via neonx_core.h, mas por garantia)
#include <stdint.h>

EMSCRIPTEN_KEEPALIVE
void neonx_wasm_render_canvas(uint8_t* buffer, int width, int height, int mode, int32_t phase) {
    int32_t cx = FLOAT_TO_FIXED(width / 2.0f);
    int32_t cy = FLOAT_TO_FIXED(height / 2.0f);
    float diag = sqrtf((float)width * width + (float)height * height);
    int32_t max_dist = FLOAT_TO_FIXED(diag / 2.0f);

    for (int y = 0; y < height; y++) {
        int32_t y_fixed = FLOAT_TO_FIXED(y);
        for (int x = 0; x < width; x++) {
            int r, g, b;
            neonx_get_color(FLOAT_TO_FIXED(x), y_fixed, mode, cx, cy, max_dist, phase, &r, &g, &b);
            
            // Mapeia coordenadas X/Y para o array 1D linear do Canvas RGBA
            int idx = (y * width + x) * 4;
            buffer[idx] = r;
            buffer[idx+1] = g;
            buffer[idx+2] = b;
            buffer[idx+3] = 255; // Alpha total
        }
    }
}
