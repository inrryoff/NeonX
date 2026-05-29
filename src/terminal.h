#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>

#define MAX_LINE_LEN 4096

typedef struct {
    wchar_t **lines;
    int count;
    int max_line_len;
} Content;

void set_integrity_status(int status);
void sleep_us(uint32_t microseconds);
void set_content_initialized(void);
void free_content(Content *c);
void print_version(void);
void print_license(void);
void show_help(void);
void handle_sigint(int sig);
void terminal_setup_signals(Content *c);

#endif
