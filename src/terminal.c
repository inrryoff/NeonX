#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE
#include "integrity.h"
#include "terminal.h"
#include "render.h"
#include "msgs.h"
#include "style.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <signal.h>
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
#else
#include <unistd.h>
#endif

#if defined(__linux__)
extern char *program_invocation_short_name;
#define PROG_NAME program_invocation_short_name
#elif defined(__APPLE__) || defined(__FreeBSD__)
#include <stdlib.h>
#define PROG_NAME getprogname()
#else
#define PROG_NAME "neonx"
#endif


static int g_integrity_status = -1;
static Content *g_current_content = NULL;
static const char* ORIGINAL_CREATOR = "@inrryoff";

#ifndef BUILD_MAINTAINER
#define BUILD_MAINTAINER "Unspecified"
#endif
#ifndef VERSION
#define VERSION "Unspecified"
#endif
#ifndef BUILD_STATUS
#define BUILD_STATUS "Unspecified"
#endif

/** Define o status global de integridade do binário. */
void set_integrity_status(int status) {
    g_integrity_status = status;
}

/** Suspende a execução por um intervalo de microssegundos. */
void sleep_us(uint32_t microseconds)
{
#ifdef _WIN32
    Sleep(microseconds / 1000);
#else
    usleep(microseconds);
#endif
}

/** Libera a memória alocada para as linhas de texto processadas. */
void free_content(Content *c) {
    if (!c) return;
    if (c->lines) {
        for (int i = 0; i < c->count; i++) {
            if (c->lines[i]) {
                free(c->lines[i]);
                c->lines[i] = NULL;
            }
        }
        free(c->lines);
        c->lines = NULL;
    }
    if (c->line_lens) {
        free(c->line_lens);
        c->line_lens = NULL;
    }
    c->count = 0;
}

/** Exibe a versão do programa, criador e status de integridade. */
void print_version(void) {
    printf("\033[1;31mN\033[1;33me\033[1;32mo\033[1;36mn\033[1;34mX\033[0m v%s\n", VERSION);
    printf("%s%s\n", MSG(MSG_VERSION_ORIGINAL_CREATOR), ORIGINAL_CREATOR);
    printf("%s%s\n", MSG(MSG_VERSION_COMPILED_BY), BUILD_MAINTAINER);
    
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

    if (g_integrity_status == 0) {
        if (is_using_official_key()) {
            printf(MSG(MSG_VERSION_STATUS_OFFICIAL), "OFFICIAL_BY_INRRYOFF");
        } else {
            printf(MSG(MSG_VERSION_STATUS_VERIFIED_BY), BUILD_STATUS);
        }
    } else if (g_integrity_status == 2) {
        printf("%s", MSG(MSG_VERSION_STATUS_ERROR));
    } else {
        printf("%s", MSG(MSG_VERSION_STATUS_MODIFIED));
    }

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

/** Exibe o texto completo da licença de uso. */
void print_license(void)
{
    printf("%s", MSG(MSG_LICENSE_TEXT));
}

/** Exibe o menu de ajuda com todas as opções de linha de comando. */
void show_help(void)
{
    printf("%s" "v%s | %s%s | %s%s\n\n", 
           MSG(MSG_HELP_HEADER), VERSION, 
           MSG(MSG_VERSION_ORIGINAL_CREATOR), ORIGINAL_CREATOR,
           MSG(MSG_VERSION_COMPILED_BY), BUILD_MAINTAINER);
    
    printf("%s", MSG(MSG_HELP_USAGE));
    printf("%s", MSG(MSG_HELP_M));
    printf("%s", MSG(MSG_HELP_S));
    printf("%s", MSG(MSG_HELP_F));
    printf("%s", MSG(MSG_HELP_D));
    printf("%s", MSG(MSG_HELP_A));
    printf("%s", MSG(MSG_HELP_P));
    printf("%s", MSG(MSG_HELP_S_UPPER));
    printf("%s", MSG(MSG_HELP_C));
    printf("%s", MSG(MSG_HELP_O));
    printf("%s", MSG(MSG_HELP_F_UPPER));
    printf("%s", MSG(MSG_HELP_L));
    printf("%s", MSG(MSG_HELP_PRESET));
    printf("%s", MSG(MSG_HELP_COLOR1));
    printf("%s", MSG(MSG_HELP_COLOR2));
    printf("%s", MSG(MSG_HELP_QUANTIZED));
    printf("%s", MSG(MSG_HELP_SPIN));
    printf("%s", MSG(MSG_HELP_LANG));
    printf("%s", MSG(MSG_HELP_LICENSE));
    printf("%s", MSG(MSG_HELP_VERSION));
    printf("%s", MSG(MSG_HELP_HELP));
}

static bool content_initialized = false;

/** Sinaliza que a estrutura de conteúdo foi inicializada corretamente. */
void set_content_initialized(void) {
    content_initialized = true;
}

/** Manipulador de interrupção (SIGINT) para restauração segura do terminal. */
void handle_sigint(int sig) {
    (void)sig;
    const char *msg = "\033[?7h\033[?25h\033[0m\n";
    #ifndef _WIN32
    if (write(STDOUT_FILENO, msg, 16) == -1) {}
    #else
    _write(1, msg, 16);
    #endif
    
    _exit(130);
}

/** Configura os interceptadores de sinais do sistema para encerramento limpo. */
void terminal_setup_signals(Content *c) {
    g_current_content = c;
#ifndef _WIN32
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
#else
    signal(SIGINT, handle_sigint);
#endif
}

