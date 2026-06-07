/*
 * NeonX — Terminal Shader Engine
 * Copyright (C) 2026  inrryoff
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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

