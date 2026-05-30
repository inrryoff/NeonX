#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>

#define MAX_LINE_LEN 4096

/** Estrutura que armazena as linhas de texto processadas. */
typedef struct {
    wchar_t **lines;
    int count;
    int max_line_len;
} Content;

/** Define o resultado global da verificação de integridade. */
void set_integrity_status(int status);

/** Realiza uma pausa na execução por microssegundos. */
void sleep_us(uint32_t microseconds);

/** Sinaliza que a estrutura de conteúdo está pronta para uso. */
void set_content_initialized(void);

/** Libera os recursos de memória alocados para o conteúdo. */
void free_content(Content *c);

/** Exibe a versão e informações de autoria. */
void print_version(void);

/** Exibe a licença do software. */
void print_license(void);

/** Exibe o guia de uso e opções do programa. */
void show_help(void);

/** Manipulador de sinal para interrupção CTRL+C. */
void handle_sigint(int sig);

/** Configura os sinais do sistema para controle do terminal. */
void terminal_setup_signals(Content *c);

#endif

