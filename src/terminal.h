#ifndef TERMINAL_H
#define TERMINAL_H

#include <wchar.h>
#include <stdbool.h>
#include <signal.h>

#define MAX_LINES 1024
#define MAX_LINE_LEN 2048

typedef struct { 
    wchar_t *lines[MAX_LINES]; 
    int count; 
} Content;

extern Content content;

void setup_terminal(void);
void sleep_us(long microseconds);
void free_content(Content *c);
void print_version(void);
void print_license(void);
void show_help(void);
void handle_sigint(int sig);
int sigint_triggered(void);
void set_integrity_status(int status);

#endif