#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include "render.h"
#include "shaders.h"
#include "msgs.h"
#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <stdarg.h>
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
#endif

#ifdef _WIN32
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
void load_input_data(struct neonx_options *opts, Content *content_ptr)
{
    if (opts->stream_mode) return;

    wchar_t buf[MAX_LINE_LEN];
    content_ptr->count = 0;
    content_ptr->max_line_len = 0;

    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        if (content_ptr->count >= (g_max_lines_limit - 1)) {
            fprintf(stderr, "%s", MSG(MSG_ERR_LEN_LIMIT));
            sleep_us(1000000); // 1s is enough
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

        content_ptr->lines[content_ptr->count++] = dup_buf;
    }
    
    if (content_ptr->count == 0 && !opts->stream_mode) {
        fprintf(stderr, "%s", MSG(MSG_ERR_SEM_DADOS));
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

    if ((size_t)n >= rem) {
        return rem - 1;
    }

    return (size_t)n;
}

static void print_colored_line(wchar_t *line, int32_t y_fixed, int32_t phase, struct neonx_options *opts, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, char **buf_ptr, size_t *rem_ptr)
{
    if (!line) return;
    int line_len = (int)wcsnlen(line, MAX_LINE_LEN);
    int last_r = -1, last_g = -1, last_b = -1;
    bool use_buffer = (buf_ptr && rem_ptr);

    bool need_fallback = false;
    if (wcstombs(NULL, line, 0) == (size_t)-1) {
        need_fallback = true;
    }

    for (int x = 0; x < line_len; x++) {
        int r = 0, g = 0, b = 0;
        get_color_fast(FLOAT_TO_FIXED(x), y_fixed, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);

        char mb[16] = {0};
        int mb_len = 0;
        if (need_fallback) {
            mb_len = wctomb(mb, line[x]);
            if (mb_len <= 0) { mb[0] = '?'; mb_len = 1; }
        }

        if (line[x] != L' ' && line[x] != L'\t') {
            if (r != last_r || g != last_g || b != last_b) {
                if (use_buffer) {
                    size_t w = safe_append(*buf_ptr, *rem_ptr, "\033[38;2;%d;%d;%dm", r, g, b);
                    *buf_ptr += w; *rem_ptr -= w;
                } else {
                    printf("\033[38;2;%d;%d;%dm", r, g, b);
                }
                last_r = r; last_g = g; last_b = b;
            }
        }

        if (use_buffer) {
            if (need_fallback) {
                for(int k=0; k<mb_len; k++) {
                    if (*rem_ptr > 1) { **buf_ptr = mb[k]; (*buf_ptr)++; (*rem_ptr)--; }
                }
            } else {
                size_t w = safe_append(*buf_ptr, *rem_ptr, "%lc", line[x]);
                *buf_ptr += w; *rem_ptr -= w;
            }
        } else {
            if (need_fallback) {
                for(int k=0; k<mb_len; k++) putchar(mb[k]);
            } else {
                printf("%lc", line[x]);
            }
        }
    }
}

int run_stream_mode(struct neonx_options *opts, Content *content_ptr)
{
    int line_count = content_ptr->count;
    if (write(STDOUT_FILENO, "\033[?7l\033[?25l", 11) == -1) return 1;
    int exit_status = 0;
    int32_t phase = opts->phase_fixed;

    for (int i = 0; i < line_count; i++) {
        wchar_t *line_buf = content_ptr->lines[i];
        size_t len = wcslen(line_buf);
        int32_t y_fixed = FLOAT_TO_FIXED(i);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED((float)len / 2.0f);
        int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);

        print_colored_line(line_buf, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, NULL, NULL);

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
        
        sanitize_ansi_escapes_w(buf);

        int32_t y_fixed = FLOAT_TO_FIXED(line_count);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED((float)len / 2.0f);
        int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);

        print_colored_line(buf, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, NULL, NULL);

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

int run_buffered_mode(struct neonx_options *opts, Content *content_ptr) {
    if (content_ptr->count <= 0) return 0;
    
    int max_w = 0;
    for(int i = 0; i < content_ptr->count; i++) {
        if (!content_ptr->lines[i]) continue;
        int l = (int)wcsnlen(content_ptr->lines[i], MAX_LINE_LEN);
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
    int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
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
            int32_t y_fixed = FLOAT_TO_FIXED(y);

            w = safe_append(ptr, rem, "\r");
            ptr += w; rem -= w;

            print_colored_line(line, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, &ptr, &rem);

            w = safe_append(ptr, rem, "\033[0m\033[K\n");
            ptr += w; rem -= w;
        }

        w = safe_append(ptr, rem, "\033[?2026l");
        ptr += w; rem -= w;

        if (write(STDOUT_FILENO, frame_buf, (size_t)(ptr - frame_buf)) == -1) {
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
