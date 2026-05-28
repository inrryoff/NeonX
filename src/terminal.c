// ==================== terminal.c ====================
#include "terminal.h"
#include "msgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#define write _write // Unificação de compatibilidade POSIX p/ compiladores no Windows
#else
#include <unistd.h>
#endif

// Instância global que contém o documento textual lido.
Content content = {.count = 0};

// O valor inicial (2) significa "Desconhecido / Em andamento" da validação
static int g_integrity_status = 2;

// Metadados que são exibidos nos comandos informativos `--version`
const char* ORIGINAL_CREATOR = "@inrryoff";

// Macros que costumam ser introduzidas de fora via compilação (Make/CMake)
#ifndef BUILD_MAINTAINER
#define BUILD_MAINTAINER "Unspecified"
#endif
#ifndef VERSION
#define VERSION "Unspecified"
#endif

/**
 * Nome da função: set_integrity_status
 * O que faz: Atualiza o status atual para permitir que `--version` avise o usuário da quebra.
 * Como funciona: Apenas sobrescreve o inteiro.
 */
void set_integrity_status(int status)
{
    g_integrity_status = status;
}

/**
 * Nome da função: sleep_us
 * O que faz: Congela o programa intencionalmente por uma fração de tempo para criar "quadros (FPS)".
 * Como funciona: Identifica qual SO está sendo usado e chama o método de pausa do sistema.
 * Parâmetros: microseconds: Um milhão equivale a 1 segundo de pausa.
 * Retorno: Nenhum.
 * Onde é usada: Na função run_buffered_mode do Render.c entre cada impressão de quadro.
 */
void sleep_us(uint32_t microseconds)
{
#ifdef _WIN32
    Sleep(microseconds / 1000); // Windows só aceita milisegundos nativamente no Sleep()
#else
    usleep(microseconds); // Unix entende microssegundos nativamente com a usleep()
#endif
}

/**
 * Nome da função: free_content
 * O que faz: Limpa e destrói o texto que estava salvo da memória RAM (Gabbage Collection manual).
 * Como funciona: Faz um laço em todas as linhas lidas com o comando `free()`.
 * Parâmetros: O ponteiro pro `Content` (O livro na memória).
 * Retorno: Nenhum.
 * Onde é usada: Chamada ao desligar o programa.
 */
void free_content(Content *c)
{
    for(int i=0; i<c->count; i++) if(c->lines[i]) free(c->lines[i]);
    c->count = 0;
}

/**
 * Nome da função: print_version
 * O que faz: Exibe o nome do desenvolvedor e se esse programa não sofreu ataques maliciosos ou injetores.
 */
void print_version(void)
{
    printf("NeonX v%s\n", VERSION);
    printf(MSG(MSG_VERSION_ORIGINAL_CREATOR), ORIGINAL_CREATOR);
    printf(MSG(MSG_VERSION_COMPILED_BY), BUILD_MAINTAINER);
    if (g_integrity_status == 0) {
        printf("%s", MSG(MSG_VERSION_STATUS_OFFICIAL));
    } else if (g_integrity_status == 2) {
        printf("%s", MSG(MSG_VERSION_STATUS_ERROR));
    } else {
        printf("%s", MSG(MSG_VERSION_STATUS_MODIFIED));
    }
}

/**
 * Nome da função: print_license
 * O que faz: Invoca o dicionário para imprimir o longo texto de licença de código aberto.
 */
void print_license(void)
{
    printf("%s", MSG(MSG_LICENSE_TEXT));
}

/**
 * Nome da função: show_help
 * O que faz: Exibe o menu de ajuda completo para o usuário, no idioma detectado ou forçado.
 * Como funciona: Invoca a macro MSG sequencialmente pegando cada linha do menu formatada do arquivo msgs.c.
 * Parâmetros: Nenhum.
 * Retorno: Nenhum.
 * Onde é usada: No arquivo main.c, acionada pelas flags -h, --help ou em casos de erro de sintaxe.
 */
// ==================== terminal.c ====================
void show_help(void)
{
    printf(MSG(MSG_HELP_HEADER), VERSION, ORIGINAL_CREATOR, BUILD_MAINTAINER);
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
    printf("%s", MSG(MSG_HELP_QUANTIZED));
    printf("%s", MSG(MSG_HELP_SPIN));
    printf("%s", MSG(MSG_HELP_LANG));
    printf("%s", MSG(MSG_HELP_LICENSE));
    printf("%s", MSG(MSG_HELP_VERSION));
    printf("%s", MSG(MSG_HELP_HELP));
}

/**
 * Nome da função: handle_sigint
 * O que faz: Restaura o terminal à força caso o usuário seja impaciente e feche o app no meio executando CTRL + C.
 * Como funciona: Chamada pelo Sistema Operacional de forma Assíncrona via event listener.
 * Parâmetros: sig: o código de interrupção (ex: 2 = SIGINT).
 * Retorno: Sai abruptamente (exit 130).
 * Onde é usada: No arquivo main() tem a linha `signal(SIGINT, handle_sigint)`.
 * Observações: A escrita "\033[?7h\033[?25h\033[0m\n" reativa o cursor invisível na tela e reseta as cores do cmd.
 */
static bool content_initialized = false;
void set_content_initialized(void)
{
    content_initialized = true;
}

void handle_sigint(int sig)
{
    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16);
    if (content_initialized) {
        free_content(&content);
    }
    exit(130);
}
