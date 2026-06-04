#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include "neonx.h"
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

static bool is_stdout_terminal(void) {
#ifdef _WIN32
    return _isatty(_fileno(stdout));
#else
    return isatty(fileno(stdout));
#endif
}

#define NX_BUILD_CTX_ID nx_get_build_id_context()

#ifndef BUILD_MAINTAINER
#define BUILD_MAINTAINER "Unspecified"
#endif
#ifndef VERSION
#define VERSION "Unspecified"
#endif
#ifndef BUILD_STATUS
#define BUILD_STATUS "Unspecified"
#endif

void set_integrity_status(int status) {
    g_integrity_status = status;
}

void sleep_us(uint32_t microseconds)
{
#ifdef _WIN32
    Sleep(microseconds / 1000);
#else
    usleep(microseconds);
#endif
}

void free_content(Content *c) {
    if (!c) return;
    if (c->pool) {
        free(c->pool);
        c->pool = NULL;
    } else if (c->lines) {
        for (int i = 0; i < c->count; i++) {
            if (c->lines[i]) {
                free(c->lines[i]);
                c->lines[i] = NULL;
            }
        }
    }
    if (c->lines) {
        free(c->lines);
        c->lines = NULL;
    }
    if (c->line_lens) {
        free(c->line_lens);
        c->line_lens = NULL;
    }
    c->count = 0;
}

void print_version(bool disable_ansi) {
    if (!disable_ansi && !is_stdout_terminal()) disable_ansi = true;
    uint32_t bid = nx_fixed_math_validate_sync(NX_BUILD_CTX_ID) ? NX_SYNC_ID : 0;
    char build_tag[32];
    
    if (bid == 0) {
        snprintf(build_tag, sizeof(build_tag), "0000-FX00");
    } else {
        /* Vetor de transformação para representação semântica de build */
        uint32_t obscure = (bid ^ 0xA5A5A5A5) + 0xDEADBEEF;
        snprintf(build_tag, sizeof(build_tag), "%X.%X.%X", 
                (obscure >> 16) & 0xFF, (obscure >> 8) & 0xFF, obscure & 0xFF);
    }

    if (disable_ansi) {
        printf("NeonX v%s [Build ID: %s]\n", VERSION, build_tag);
    } else {
        printf("%s v%s [Build ID: %s%s%s]\n", 
               LOGO_NEONX, VERSION, MSG_NUMBER, build_tag, RESET);
    }
    printf("%s%s\n", MSG_F(MSG_VERSION_NX_BUILD_CTX_ID, disable_ansi), NX_BUILD_CTX_ID);
    printf("%s%s\n", MSG_F(MSG_VERSION_COMPILED_BY, disable_ansi), BUILD_MAINTAINER);
    
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

    if (g_integrity_status == 0) {
        if (is_using_official_key()) {
            printf(MSG_F(MSG_VERSION_STATUS_OFFICIAL, disable_ansi), "OFFICIAL_BY_INRRYOFF");
        } else {
            printf(MSG_F(MSG_VERSION_STATUS_VERIFIED_BY, disable_ansi), BUILD_STATUS);
        }
    } else if (g_integrity_status == 2) {
        printf("%s", MSG_F(MSG_VERSION_STATUS_ERROR, disable_ansi));
    } else {
        printf("%s", MSG_F(MSG_VERSION_STATUS_MODIFIED, disable_ansi));
    }

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

void print_license(bool disable_ansi)
{
    if (!disable_ansi && !is_stdout_terminal()) disable_ansi = true;
    printf("%s", MSG_F(MSG_LICENSE_TEXT, disable_ansi));
}

void show_help(bool disable_ansi)
{
    if (!disable_ansi && !is_stdout_terminal()) disable_ansi = true;
    printf("%s" "v%s | %s%s | %s%s\n\n", 
           MSG_F(MSG_HELP_HEADER, disable_ansi), VERSION, 
           MSG_F(MSG_VERSION_NX_BUILD_CTX_ID, disable_ansi), NX_BUILD_CTX_ID,
           MSG_F(MSG_VERSION_COMPILED_BY, disable_ansi), BUILD_MAINTAINER);
    
    printf("%s", MSG_F(MSG_HELP_TEXT, disable_ansi));
}

static bool content_initialized = false;
void set_content_initialized(void) { content_initialized = true; }

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

void terminal_setup_signals(Content *c) {
    g_current_content = c;
#ifndef _WIN32
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
#else
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
#endif
}
