#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef _WIN32
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <unistd.h>
#endif

Content content = {.count = 0};

#ifndef VERSION
    #define VERSION "2.0.1-MOD"
#endif
#ifndef BUILD_MAINTAINER
    #define BUILD_MAINTAINER "Local"
#endif
#ifndef BUILD_STATUS
    #define BUILD_STATUS "UNOFFICIAL"
#endif

extern const char* ORIGINAL_CREATOR;

void setup_terminal(void) {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return;
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    #endif
}

void sleep_us(long microseconds) {
    if (microseconds <= 0) return;
    #ifdef _WIN32
        DWORD ms = (microseconds + 999) / 1000; 
        Sleep(ms);
    #else
        usleep((useconds_t)microseconds);
    #endif
}

void free_content(Content *c) {
    for(int i=0; i<c->count; i++) if(c->lines[i]) free(c->lines[i]);
    c->count = 0;
}

static volatile sig_atomic_t sigint_received = 0;

void handle_sigint(int sig) {
    sigint_received = 1;
}

int sigint_triggered(void) {
    return sigint_received != 0;
}

static int g_integrity_status = 2;

void set_integrity_status(int status) {
    g_integrity_status = status;
}

void print_version(void) {
    if (g_integrity_status == 0) {
        printf("NeonX v%s\n", VERSION);
        printf("Criador Original: %s\n", "@inrryoff");
        printf("Compilado por: %s\n", BUILD_MAINTAINER);
        printf("Status: %s\n", BUILD_STATUS);
    } else {
        printf("NeonX (versão modificada)\n");
        printf("Criador Original: DESCONHECIDO\n");
        printf("Compilado por: DESCONHECIDO\n");
        printf("Status: MODIFICADO (não oficial)\n");
    }
}

void print_license(void) {
    wprintf(L"LICENÇA DE USO - NEONX (C - VERSION)\n");
    wprintf(L"-----------------------------------------------------------------\n");
    wprintf(L"Copyright (c) 2024 @inrryoff - Licenciado sob condições especiais NeonX LICENSE\n\n");
    wprintf(L"Pelo presente, fica concedida permissão a qualquer pessoa que obtenha uma cópia\n");
    wprintf(L"deste software para usá-lo gratuitamente, sujeito às seguintes condições:\n\n");  
    wprintf(L"1. ATRIBUIÇÃO (CRÉDITOS):\n");
    wprintf(L"   O nome do autor original (@inrryoff) e os avisos de copyright devem ser\n");
    wprintf(L"   mantidos em todos os arquivos de código-fonte, cabeçalhos e na saída de\n");
    wprintf(L"   versão do binário compilado (ex: neonx --version).\n\n");  
    wprintf(L"2. PROIBIÇÃO DE COMERCIALIZAÇÃO:\n");
    wprintf(L"   É TERMINANTEMENTE PROIBIDA a venda, aluguel ou qualquer forma de\n");
    wprintf(L"   comercialização deste software, seja do código-fonte ou do binário\n");
    wprintf(L"   compilado, de forma isolada ou integrada a pacotes pagos.\n\n");  
    wprintf(L"3. DERIVAÇÕES E MODIFICAÇÕES:\n");
    wprintf(L"   Alterações no código são permitidas para melhorias ou uso pessoal, desde que:\n");
    wprintf(L"   a) O trabalho derivado NÃO seja vendido.\n");
    wprintf(L"   b) A versão modificada seja mantida em repositório público (Open Source).\n");
    wprintf(L"   c) Os créditos ao autor original sejam mantidos de forma clara.\n\n");  
    wprintf(L"4. DISTRIBUIÇÃO EM MÓDULOS (MAGISK/KERNELSU):\n");
    wprintf(L"   O uso deste binário em módulos de otimização é permitido e encorajado,\n");
    wprintf(L"   desde que o módulo seja distribuído gratuitamente.\n\n");
    wprintf(L"O SOFTWARE É FORNECIDO 'COMO ESTÁ', SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU\n");
    wprintf(L"IMPLÍCITA. EM NENHUM EVENTO O AUTOR SERÁ RESPONSÁVEL POR QUALQUER RECLAMAÇÃO,\n");  
    wprintf(L"DANOS OU OUTRA RESPONSABILIDADE RESULTANTE DO USO DESTE SOFTWARE.\n");
}


void show_help(void) {
    wprintf(L"NeonX v%s | Core por: %s | Build por: %s\n\n", VERSION, ORIGINAL_CREATOR, BUILD_MAINTAINER);
    wprintf(L"Uso: cat arquivo | neonx [opcoes]\n\n");
    wprintf(L"-m [0-11]      Modos de animação\n");
    wprintf(L"-s [valor]     Velocidade (0.2 padrao)\n");
    wprintf(L"-f [valor]     Frequência (0.3 padrao)\n");
    wprintf(L"-d [valor]     Duração (0: infinito)\n");
    wprintf(L"-D [valor]     Inclinação diagonal\n");
    wprintf(L"-p [valor]     Seeds fixas\n");
    wprintf(L"-S             Modo estático\n");
    wprintf(L"-c [largura]   Largura fixa do gradiente\n");
    wprintf(L"-o [0-1]       Opacidade das bordas\n");
    wprintf(L"-F [valor]     FPS (ex: 60, 90)\n");
    wprintf(L"-L             Modo linha por linha (stream)\n");
    wprintf(L"--preset [nome] Carregar preset (cyberpunk, retro, matrix, sunset)\n");
    wprintf(L"--termux       Modo de compatibilidade com Termux\n");
    wprintf(L"--spin         Lista de cores para spinner\n");
    wprintf(L"--license      Licença de software\n");
    wprintf(L"-v,--version   Versão do binário\n");
    wprintf(L"-h,--help      Exibe esta ajuda\n");
}
