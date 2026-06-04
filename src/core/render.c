#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
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

void cleanup_and_exit(Content *content_ptr, int exit_code)
{
    if (write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16) == -1) {
    }
    free_content(content_ptr);
    exit(exit_code);
}

static void sanitize_ansi_escapes_w(wchar_t *buf) {
    if (!buf) return;
    size_t len = wcslen(buf);
    for (size_t i = 0; i < len; i++) {
        if (buf[i] == 0x1B) buf[i] = L'?';
    }
}

void load_input_data(struct neonx_options *opts, Content *content_ptr) {
#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
#else
    if (isatty(fileno(stdin))) {
#endif
        fprintf(stderr, "%s", MSG(MSG_ERR_NO_DATA));
        cleanup_and_exit(content_ptr, 1);
    }

    if (opts->stream_mode) return;

    wchar_t buf[MAX_LINE_LEN];
    content_ptr->count = 0;
    content_ptr->max_line_len = 0;
    
    size_t pool_cap = 131072;
    content_ptr->pool = malloc(pool_cap * sizeof(wchar_t));
    if (!content_ptr->pool) {
        fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
        cleanup_and_exit(content_ptr, 6);
    }
    size_t pool_used = 0;

    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        if (content_ptr->count >= (g_max_lines_limit - 1)) {
            fprintf(stderr, "%s", MSG(MSG_ERR_FILE_TOO_LARGE));
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

        if (pool_used + len + 1 > pool_cap) {
            size_t new_cap = pool_cap * 2;
            if (new_cap < pool_used + len + 1) new_cap = pool_used + len + 1;
            wchar_t *new_pool = realloc(content_ptr->pool, new_cap * sizeof(wchar_t));
            if (!new_pool) {
                fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
                cleanup_and_exit(content_ptr, 6);
            }
            if (new_pool != content_ptr->pool) {
                for (int i = 0; i < content_ptr->count; i++) {
                    content_ptr->lines[i] = new_pool + (content_ptr->lines[i] - content_ptr->pool);
                }
                content_ptr->pool = new_pool;
            }
            pool_cap = new_cap;
        }

        wchar_t *target = content_ptr->pool + pool_used;
        memcpy(target, buf, (len + 1) * sizeof(wchar_t));
        
        content_ptr->line_lens[content_ptr->count] = len;
        content_ptr->lines[content_ptr->count++] = target;
        pool_used += (len + 1);
    }
    
    if (content_ptr->count == 0 && !opts->stream_mode) {
        fprintf(stderr, "%s", MSG(MSG_ERR_NO_DATA));
        cleanup_and_exit(content_ptr, 0);
    }
}

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
    size_t written = (size_t)n < rem ? (size_t)n : rem - 1;
    return written;
}

typedef struct {
    char **buf_ptr;
    size_t *rem_ptr;
    int last_r, last_g, last_b;
} CliDriverCtx;

static void cli_set_color(RenderDriver *self, int r, int g, int b) {
    CliDriverCtx *ctx = (CliDriverCtx*)self->ctx;
    if (r == ctx->last_r && g == ctx->last_g && b == ctx->last_b) return;
    if (ctx->buf_ptr && ctx->rem_ptr && *ctx->rem_ptr > 0) {
        int n = snprintf(*ctx->buf_ptr, *ctx->rem_ptr, "\033[38;2;%d;%d;%dm", r, g, b);
        if (n > 0) {
            size_t written = (size_t)n;
            if (written >= *ctx->rem_ptr) written = *ctx->rem_ptr - 1;
            *ctx->buf_ptr += written;
            *ctx->rem_ptr -= written;
        }
    } else {
        printf("\033[38;2;%d;%d;%dm", r, g, b);
    }
    ctx->last_r = r; ctx->last_g = g; ctx->last_b = b;
}

static void cli_reset_color(RenderDriver *self) {
    CliDriverCtx *ctx = (CliDriverCtx*)self->ctx;
    if (ctx->buf_ptr && ctx->rem_ptr && *ctx->rem_ptr > 0) {
        int n = snprintf(*ctx->buf_ptr, *ctx->rem_ptr, "\033[0m");
        if (n > 0) {
            size_t written = (size_t)n;
            if (written >= *ctx->rem_ptr) written = *ctx->rem_ptr - 1;
            *ctx->buf_ptr += written;
            *ctx->rem_ptr -= written;
        }
    } else {
        printf("\033[0m");
    }
}

static void cli_put_char(RenderDriver *self, wchar_t c) {
    CliDriverCtx *ctx = (CliDriverCtx*)self->ctx;
    if (ctx->buf_ptr && ctx->rem_ptr && *ctx->rem_ptr > 0) {
        int n = snprintf(*ctx->buf_ptr, *ctx->rem_ptr, "%lc", (wint_t)c);
        if (n > 0) {
            size_t written = (size_t)n;
            if (written >= *ctx->rem_ptr) written = *ctx->rem_ptr - 1;
            *ctx->buf_ptr += written;
            *ctx->rem_ptr -= written;
        }
    } else {
        printf("%lc", (wint_t)c);
    }
}

static void print_colored_line(wchar_t *line, size_t line_len, int32_t y_fixed, int32_t phase, struct neonx_options *opts, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, char **buf_ptr, size_t *rem_ptr)
{
    CliDriverCtx ctx = {buf_ptr, rem_ptr, -1, -1, -1};
    RenderDriver driver = {cli_set_color, cli_reset_color, cli_put_char, &ctx};
    neonx_render_line(line, line_len, y_fixed, phase, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, &driver);
}

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
    }
    content_ptr->count = 0;

    wchar_t buf[MAX_LINE_LEN];
    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        len = wcslen(buf);
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
    
    size_t buf_size = (size_t)(max_w + 1) * (size_t)content_ptr->count * 48 + 2048;
    
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
            cleanup_and_exit(content_ptr, 1);
        }
        if (opts->static_mode) break;
        phase += opts->speed_fixed;
        sleep_us(opts->frame_time_us);
    }
    free(frame_buf);
    return 0;
}

