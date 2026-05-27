#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include "render.h"
#include "shaders.h"
#include "msgs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <unistd.h>

#ifdef _WIN32
    #include <windows.h>
    #ifndef STDOUT_FILENO
        #define STDOUT_FILENO 1
    #endif
    #define write _write 
#endif

static uint64_t get_time_us(void) {
#ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    return (uint64_t)((now.QuadPart * 1000000LL) / freq.QuadPart);
#else
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
#endif
}

/**
 * Restaura o terminal para o padrao e limpa memoria.
 */
void cleanup_and_exit(Content *content_ptr, int exit_code) {
    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16);
    free_content(content_ptr);
    exit(exit_code);
}

/**
 * Le a entrada de texto e trata limitacoes para evitar crashes.
 */
void load_input_data(struct neonx_options *opts, Content *content_ptr) {
    if (opts->stream_mode) return;

    wchar_t buf[MAX_LINE_LEN];
    content_ptr->count = 0;

    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        if (content_ptr->count >= (MAX_LINES - 1)) {
            fprintf(stderr, "%s", MSG(MSG_ERR_LEN_LIMIT));
            sleep_us(2000000); 
            opts->stream_mode = true;
            break; 
        }

        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; }
        
        content_ptr->lines[content_ptr->count++] = wcsdup(buf);
    }
}

/**
 * Executa o aplicativo de linha em linha (modo infinito/log).
 */
int run_stream_mode(struct neonx_options *opts, Content *content_ptr) {
    int line_count = content_ptr->count;
    write(STDOUT_FILENO, "\033[?7l\033[?25l", 11);
    int exit_status = 0;
    int32_t phase = opts->phase_fixed;

    for (int i = 0; i < line_count; i++) {
        wchar_t *line_buf = content_ptr->lines[i];
        size_t len = wcslen(line_buf);
        int32_t y_fixed = FLOAT_TO_FIXED(i);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED(len / 2.0f);
        int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
        
        for (size_t x = 0; x < len; x++) {
            int r, g, b;
            int32_t x_fixed = FLOAT_TO_FIXED(x);
            get_color_fast(x_fixed, y_fixed, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
            printf("\033[38;2;%d;%d;%dm%lc", r, g, b, line_buf[x]);
        }
        printf("\033[0m\n");
        fflush(stdout);
        phase += opts->speed_fixed;
        free(content_ptr->lines[i]); 
    }
    content_ptr->count = 0;

    wchar_t buf[MAX_LINE_LEN];
    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; }
        
        int32_t y_fixed = FLOAT_TO_FIXED(line_count);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED(len / 2.0f);
        int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
        
        for (size_t x = 0; x < len; x++) {
            int r, g, b;
            int32_t x_fixed = FLOAT_TO_FIXED(x);
            get_color_fast(x_fixed, y_fixed, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
            printf("\033[38;2;%d;%d;%dm%lc", r, g, b, buf[x]);
        }
        printf("\033[0m\n");
        fflush(stdout);
        phase += opts->speed_fixed;
        line_count++;
        if (ferror(stdin)) {
            exit_status = 1;
            break;
        }
    }
    return exit_status;
}

/**
 * Executa as animacoes preenchendo o buffer de quadro inteiro.
 */
int run_buffered_mode(struct neonx_options *opts, Content *content_ptr) {
    if (content_ptr->count == 0) return 0;
    
    int max_w = 0;
    for(int i = 0; i < content_ptr->count; i++) { 
        int l = wcslen(content_ptr->lines[i]); 
        if(l > max_w) max_w = l;
    }
    if (opts->fixed_width > 0) max_w = opts->fixed_width;
    
    size_t buf_size = content_ptr->count * (max_w * 32 + 100);
    char *frame_buf = malloc(buf_size);
    if (!frame_buf) {
        return 6;
    }
    
    #define SAFE_APPEND(...) do { size_t rem = buf_size - (ptr - frame_buf); int n = snprintf(ptr, rem, __VA_ARGS__); if (n > 0 && (size_t)n < rem) ptr += n; } while(0)
    
    int32_t phase = opts->phase_fixed;
    write(STDOUT_FILENO, "\033[?25l\033[?7l", 11);
    bool first_frame = true;
    int32_t cx_fixed = FLOAT_TO_FIXED(max_w / 2.0f);
    int32_t cy_fixed = FLOAT_TO_FIXED(content_ptr->count / 2.0f);
    int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
    uint64_t start_time_us = get_time_us();

    while (1) {
        if (opts->duration_us > 0) {
            uint64_t now_us = get_time_us();
            if (now_us - start_time_us >= opts->duration_us) break;
        }
        char *ptr = frame_buf;
        SAFE_APPEND("\033[?2026h");
        if (!first_frame) { 
            if (content_ptr->count > 100) {
                SAFE_APPEND("\033[H"); 
            } else {
                SAFE_APPEND("\033[%dA", content_ptr->count); 
            }
        }
        first_frame = false;
        
        for (int y = 0; y < content_ptr->count; y++) {
            wchar_t *line = content_ptr->lines[y];
            int line_len = wcslen(line);
            int32_t y_fixed = FLOAT_TO_FIXED(y);
            SAFE_APPEND("\r");
            int last_r = -1, last_g = -1, last_b = -1;
            
            for (int x = 0; x < line_len; x++) {
                int r, g, b;
                int32_t x_fixed = FLOAT_TO_FIXED(x);
                get_color_fast(x_fixed, y_fixed, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
                if (line[x] == L' ' || line[x] == L'\t') {
                    SAFE_APPEND("%lc", line[x]);
                } else {
                    if (r != last_r || g != last_g || b != last_b) {
                        SAFE_APPEND("\033[38;2;%d;%d;%dm", r, g, b);
                        last_r = r; last_g = g; last_b = b;
                    }
                    SAFE_APPEND("%lc", line[x]);
                }
            }
            SAFE_APPEND("\033[0m\033[K\n");
        }
        SAFE_APPEND("\033[?2026l");
        write(STDOUT_FILENO, frame_buf, ptr - frame_buf);
        
        if (opts->static_mode) break;
        phase += opts->speed_fixed;
        sleep_us(opts->frame_time_us);
    }

    free(frame_buf);
    return 0;
}