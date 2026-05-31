#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include "render.h"
#include "msgs.h"
#include "terminal.h"
#include "neonx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#define write _write
#define wcsdup _wcsdup
#endif

extern int g_max_lines_limit;

/** Obtém o tempo atual do sistema em microssegundos para controle de quadros. */
static uint64_t get_time_us(void)
{
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

/** Restaura as configurações do terminal, libera recursos e encerra o processo. */
void cleanup_and_exit(Content *content_ptr, int exit_code)
{
    if (write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16) == -1) {
    }
    free_content(content_ptr);
    exit(exit_code);
}

/** Remove caracteres de escape ANSI de uma string para prevenir corrupção visual. */
static void sanitize_ansi_escapes_w(wchar_t *buf) {
    if (!buf) return;
    size_t len = wcslen(buf);
    for (size_t i = 0; i < len; i++) {
        if (buf[i] == 0x1B) buf[i] = L'?';
    }
}

/** Lê os dados da entrada padrão e os armazena na estrutura de conteúdo. */
void load_input_data(struct neonx_options *opts, Content *content_ptr)
{
    if (opts->stream_mode) return;

    wchar_t buf[MAX_LINE_LEN];
    content_ptr->count = 0;
    content_ptr->max_line_len = 0;

    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        if (content_ptr->count >= (g_max_lines_limit - 1)) {
            fprintf(stderr, "%s", MSG(MSG_ERR_LEN_LIMIT));
            sleep_us(1000000); 
            opts->stream_mode = true;
            break;
        }

        size_t len = wcsnlen(buf, MAX_LINE_LEN);
        if (len > 0 && buf[len-1] == L'\n') {
            buf[len-1] = L'\0';
            len--;
        }

        if ((int)len > content_ptr->max_line_len) {
            content_ptr->max_line_len = (int)len;
        }

        sanitize_ansi_escapes_w(buf);

        wchar_t *dup_buf = wcsdup(buf);
        if (!dup_buf) {
            fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
            cleanup_and_exit(content_ptr, 6);
        }

        content_ptr->line_lens[content_ptr->count] = len;
        content_ptr->lines[content_ptr->count++] = dup_buf;
    }
    
    if (content_ptr->count == 0 && !opts->stream_mode) {
        fprintf(stderr, "%s", MSG(MSG_ERR_SEM_DADOS));
        cleanup_and_exit(content_ptr, 0);
    }
}

/** Anexa texto formatado a um buffer com proteção contra estouro de limite. */
#ifdef __GNUC__
__attribute__((format(printf, 3, 4)))
#endif
static size_t safe_append(char *ptr, size_t rem, const char *fmt, ...)
{
    if (rem == 0) return 0;
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(ptr, rem, fmt, args);
    va_end(args);
    if (n < 0) return 0;
    if ((size_t)n >= rem) return rem - 1;
    return (size_t)n;
}

typedef struct {
    char **buf_ptr;
    size_t *rem_ptr;
    int last_r, last_g, last_b;
} CliDriverCtx;

/** Define a cor ANSI 24-bits via driver de renderização. */
static void cli_set_color(RenderDriver *self, int r, int g, int b) {
    CliDriverCtx *ctx = (CliDriverCtx*)self->ctx;
    if (r == ctx->last_r && g == ctx->last_g && b == ctx->last_b) return;
    if (ctx->buf_ptr && ctx->rem_ptr) {
        int n = snprintf(*ctx->buf_ptr, *ctx->rem_ptr, "\033[38;2;%d;%d;%dm", r, g, b);
        if (n > 0) { *ctx->buf_ptr += n; *ctx->rem_ptr -= (size_t)n; }
    } else {
        printf("\033[38;2;%d;%d;%dm", r, g, b);
    }
    ctx->last_r = r; ctx->last_g = g; ctx->last_b = b;
}

/** Reseta os atributos de cor do terminal. */
static void cli_reset_color(RenderDriver *self) {
    CliDriverCtx *ctx = (CliDriverCtx*)self->ctx;
    if (ctx->buf_ptr && ctx->rem_ptr) {
        int n = snprintf(*ctx->buf_ptr, *ctx->rem_ptr, "\033[0m");
        if (n > 0) { *ctx->buf_ptr += n; *ctx->rem_ptr -= (size_t)n; }
    } else {
        printf("\033[0m");
    }
}

/** Imprime um caractere largo utilizando o driver de renderização. */
static void cli_put_char(RenderDriver *self, wchar_t c) {
    CliDriverCtx *ctx = (CliDriverCtx*)self->ctx;
    if (ctx->buf_ptr && ctx->rem_ptr) {
        int n = snprintf(*ctx->buf_ptr, *ctx->rem_ptr, "%lc", (wint_t)c);
        if (n > 0) { *ctx->buf_ptr += n; *ctx->rem_ptr -= (size_t)n; }
    } else {
        printf("%lc", (wint_t)c);
    }
}

/** Renderiza uma linha de texto aplicando o efeito de cor configurado. */
static void print_colored_line(wchar_t *line, size_t line_len, int32_t y_fixed, int32_t phase, struct neonx_options *opts, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, char **buf_ptr, size_t *rem_ptr)
{
    CliDriverCtx ctx = {buf_ptr, rem_ptr, -1, -1, -1};
    RenderDriver driver = {cli_set_color, cli_reset_color, cli_put_char, &ctx};
    neonx_render_line(line, line_len, y_fixed, phase, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, &driver);
}

/** Executa a renderização em tempo real linha por linha (modo stream). */
int run_stream_mode(struct neonx_options *opts, Content *content_ptr)
{
    int line_count = content_ptr->count;
    if (write(STDOUT_FILENO, "\033[?7l\033[?25l", 11) == -1) return 1;
    int exit_status = 0;
    int32_t phase = opts->phase_fixed;

    for (int i = 0; i < line_count; i++) {
        wchar_t *line_buf = content_ptr->lines[i];
        size_t len = content_ptr->line_lens[i];
        int32_t y_fixed = FLOAT_TO_FIXED(i);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED((float)len / 2.0f);
        int32_t max_dist_fixed = neonx_fast_dist_fixed(cx_fixed, cy_fixed);

        print_colored_line(line_buf, len, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, NULL, NULL);

        printf("\033[0m\n");
        fflush(stdout);
        phase += opts->speed_fixed;
        phase &= 0x7FFFFFFF;
        free(content_ptr->lines[i]);
    }
    content_ptr->count = 0;

    wchar_t buf[MAX_LINE_LEN];
    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        len = wcslen(buf); // Recalculate after newline removal
        sanitize_ansi_escapes_w(buf);
        int32_t y_fixed = FLOAT_TO_FIXED(line_count);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED((float)len / 2.0f);
        int32_t max_dist_fixed = neonx_fast_dist_fixed(cx_fixed, cy_fixed);
        print_colored_line(buf, len, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, NULL, NULL);
        printf("\033[0m\n");
        fflush(stdout);
        phase += opts->speed_fixed;
        line_count++;
        if (ferror(stdin)) { exit_status = 1; break; }
    }
    return exit_status;
}

/** Gerencia a animação baseada em buffer para renderização suave de quadros. */
int run_buffered_mode(struct neonx_options *opts, Content *content_ptr) {
    if (content_ptr->count <= 0) return 0;
    int max_w = 0;
    for(int i = 0; i < content_ptr->count; i++) {
        if (!content_ptr->lines[i]) continue;
        int l = (int)content_ptr->line_lens[i];
        if(l > max_w) max_w = l;
    }
    if (opts->fixed_width > 0) max_w = opts->fixed_width;
    content_ptr->max_line_len = max_w;
    size_t buf_size = (size_t)(max_w + 1) * (size_t)content_ptr->count * 36 + 512;
    const size_t MAX_BUF = 32 * 1024 * 1024;
    if (buf_size > MAX_BUF) {
        buf_size = MAX_BUF;
        fprintf(stderr, "%s", MSG(MSG_ERR_BUFFER_TRUNCATED));
    }
    char *frame_buf = malloc(buf_size);
    if (!frame_buf) {
        fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
        return 6;
    }
    int32_t phase = opts->phase_fixed;
    if (write(STDOUT_FILENO, "\033[?25l\033[?7l", 11) == -1) {
        free(frame_buf);
        return 1;
    }
    bool first_frame = true;
    int32_t cx_fixed = FLOAT_TO_FIXED((float)max_w / 2.0f);
    int32_t cy_fixed = FLOAT_TO_FIXED((float)content_ptr->count / 2.0f);
    int32_t max_dist_fixed = neonx_fast_dist_fixed(cx_fixed, cy_fixed);
    uint64_t start_time_us = get_time_us();
    while (1) {
        if (opts->duration_us > 0) {
            uint64_t now_us = get_time_us();
            if (now_us - start_time_us >= opts->duration_us) break;
        }
        char *ptr = frame_buf;
        size_t rem = buf_size;
        size_t w;
        w = safe_append(ptr, rem, "\033[?2026h");
        ptr += w; rem -= w;
        if (!first_frame) {
            if (content_ptr->count > 100) {
                w = safe_append(ptr, rem, "\033[H");
                ptr += w; rem -= w;
            } else {
                w = safe_append(ptr, rem, "\033[%dA", content_ptr->count);
                ptr += w; rem -= w;
            }
        }
        first_frame = false;
        for (int y = 0; y < content_ptr->count; y++) {
            wchar_t *line = content_ptr->lines[y];
            size_t line_len = content_ptr->line_lens[y];
            int32_t y_fixed = FLOAT_TO_FIXED(y);
            w = safe_append(ptr, rem, "\r");
            ptr += w; rem -= w;
            print_colored_line(line, line_len, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, &ptr, &rem);
            w = safe_append(ptr, rem, "\033[0m\033[K\n");
            ptr += w; rem -= w;
        }
        w = safe_append(ptr, rem, "\033[?2026l");
        ptr += w; rem -= w;
        if (write(STDOUT_FILENO, frame_buf, (unsigned int)(ptr - frame_buf)) == -1) {
            free(frame_buf);
            exit(0);
        }
        if (opts->static_mode) break;
        phase += opts->speed_fixed;
        sleep_us(opts->frame_time_us);
    }
    free(frame_buf);
    return 0;
}

