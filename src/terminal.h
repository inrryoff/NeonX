// ==================== terminal.h ====================
#ifndef TERMINAL_H
#define TERMINAL_H

#include <wchar.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

// Proteções contra estouro de RAM/Travamentos do buffer.
// Limite máximo de linhas simultâneas guardadas na memória em uma renderização completa
#define MAX_LINES 1024
// Limite máximo de caracteres de largura em uma única linha lida do terminal
#define MAX_LINE_LEN 2048

// Estrutura do documento que fica na memória.
// linhas apontam para o local onde a string do caractere está, e count é o nr atual de linhas lidas.
typedef struct { 
    wchar_t *lines[MAX_LINES]; 
    int count; 
} Content;

// Declarada extern para ser manipulada globalmente pelo renderizador e sinalizador
extern Content content;

// Funções expostas de ajuda relativas ao ambiente (Sistema/Console/Terminal).
void set_integrity_status(int status);
void sleep_us(uint32_t microseconds);
void set_content_initialized(void);
void free_content(Content *c);
void print_version(void);
void print_license(void);
void show_help(void);
void handle_sigint(int sig);

#endif
