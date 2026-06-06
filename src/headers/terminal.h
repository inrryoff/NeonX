#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>

#define MAX_LINE_LEN 4096

typedef struct {
    wchar_t **lines;
    size_t *line_lens;
    wchar_t *pool;
    int count;
    int max_line_len;
} Content;

void set_integrity_status(int status);
void sleep_us(uint32_t microseconds);
void set_content_initialized(void);
void free_content(Content *c);
void print_version(bool disable_ansi);
void print_license(bool disable_ansi);
void show_help(bool disable_ansi);
void handle_sigint(int sig);
void terminal_setup_signals(Content *c);
ColorMode detect_color_mode(void);

#endif

