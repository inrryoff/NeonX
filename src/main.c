#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "integrity.h"
#include "shaders.h"
#include "msgs.h"
#include "render.h"
#include "math_fixed.h"
#include "render_core.h"

#ifdef _WIN32
#include <windows.h>
#endif

int auth_status;
int g_max_lines_limit = 10000;
extern uint32_t secure_random_u32(void);

/** Converte uma string para ponto fixo 16.16 com validação de erros e limites. */
static int32_t secure_str_to_fixed(const char *s, bool *ok) {
    if (!s || !*s) {
        if (ok) *ok = false;
        return 0;
    }
    char *endptr = NULL;
    errno = 0;
    double val = strtod(s, &endptr);
    if (errno != 0 || endptr == s || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
        if (ok) *ok = false;
        return 0;
    }
    
    if (val > 32767.0 || val < -32768.0) {
        if (ok) *ok = false;
        return 0;
    }

    if (ok) *ok = true;
    return FLOAT_TO_FIXED(val);
}

/** Exibe mensagens de erro formatadas no fluxo de erro padrão (stderr). */
#ifdef __GNUC__
__attribute__((format(printf, 1, 2)))
#endif
static void print_error_msg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

/** Processa argumentos numéricos específicos da linha de comando e atualiza as opções. */
static int handle_numeric_argument(const char *arg, const char *val, struct neonx_options *opts)
{
    if (!arg || !val || !opts) return 3;

    bool ok = false;
    int32_t num_fixed = secure_str_to_fixed(val, &ok);
    if (!ok) {
        print_error_msg(MSG(MSG_ERR_INVALID_NUMBER), arg, val);
        return 3;
    }

    if (!strcmp(arg, "-o")) {
        if (num_fixed < 0 || num_fixed > FIXED_ONE) {
            print_error_msg(MSG(MSG_ERR_INVALID_NUMBER), "-o", val);
            return 3;
        }
        neonx_set_opacity(num_fixed);
        return 0;
    }

    if (!strcmp(arg, "-F")) {
        int32_t fps = num_fixed / FIXED_ONE;
        if (fps <= 0) {
            print_error_msg(MSG(MSG_ERR_MUST_BE_POSITIVE), "-F");
            return 3;
        }
        opts->frame_time_us = (uint32_t)(1000000LL / fps);
        return 0;
    }

    if (!strcmp(arg, "-m")) {
        int32_t mode = num_fixed / FIXED_ONE;
        if (mode < 0 || mode > MAX_ANIM_MODE) {
            fprintf(stderr, "%s", MSG(MSG_ERR_MODE_LIMIT));
            return 4;
        }
        opts->anim_mode = (int)mode;
        return 0;
    }

    if (!strcmp(arg, "-c")) {
        int32_t width = num_fixed / FIXED_ONE;
        if (width <= 0) {
            print_error_msg(MSG(MSG_ERR_MUST_BE_POSITIVE), "-c");
            return 3;
        }
        opts->fixed_width = width;
        return 0;
    }

    if (!strcmp(arg, "-d")) {
        if (num_fixed < 0) {
            fprintf(stderr, "%s", MSG(MSG_ERR_DURATION_NEGATIVE));
            return 3;
        }
        opts->duration_fixed = num_fixed;
        opts->duration_us = ((uint64_t)num_fixed * 1000000) / FIXED_ONE;
        return 0;
    }

    if (!strcmp(arg, "-s")) opts->speed_fixed = num_fixed;
    else if (!strcmp(arg, "-f")) neonx_set_frequency(num_fixed);
    else if (!strcmp(arg, "-A")) neonx_set_gradient_angle(num_fixed);
    
    return 0;
}

/** Analisa recursivamente todos os argumentos passados via CLI. */
static int parse_arguments(int argc, char *argv[], struct neonx_options *opts) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        
        if (!strcmp(arg, "-max-lines") && i + 1 < argc) {
            bool ok = false;
            int32_t val_fixed = secure_str_to_fixed(argv[++i], &ok);
            if (!ok) {
                print_error_msg(MSG(MSG_ERR_INVALID_NUMBER), "-max-lines", argv[i]);
                return 3;
            }
            g_max_lines_limit = val_fixed / FIXED_ONE;
            if (g_max_lines_limit < 100) g_max_lines_limit = 100;
            continue;
        }
        if ((!strcmp(arg, "-P") || !strcmp(arg, "-p")) && i + 1 < argc) {
            bool ok = false;
            opts->phase_fixed = secure_str_to_fixed(argv[++i], &ok);
            opts->phase_fixed_set = ok;
            continue;
        }
        if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) { opts->show_help_flag = true; continue; }
        if (!strcmp(arg, "-v") || !strcmp(arg, "--version")) { opts->show_version_flag = true; continue; }
        if (!strcmp(arg, "--license")) { opts->show_license_flag = true; continue; }
        if (!strcmp(arg, "--verify-sig")) { opts->verify_sig_flag = true; continue; }
        if (!strcmp(arg, "--spin")) { opts->spin_flag = true; continue; }
        if (!strcmp(arg, "-S")) { opts->static_mode = true; continue; }
        if (!strcmp(arg, "-L")) { opts->stream_mode = true; continue; }
        if (!strcmp(arg, "--quantized")) { neonx_set_quantization(true); continue; }
        
        if (!strcmp(arg, "--lang") && i + 1 < argc) {
            msgs_set_language(argv[++i]);
            continue;
        }

        if (!strcmp(arg, "--preset") && i + 1 < argc) {
            const char *preset_val = argv[++i];
            if (preset_val[0] == '-') {
                print_error_msg(MSG(MSG_ERR_MISSING_VALUE), arg);
                return 3;
            }
            if (!shaders_set_preset(preset_val, &opts->anim_mode, &opts->speed_fixed)) {
                print_error_msg(MSG(MSG_ERR_INVALID_OPTION), preset_val);
                return 3;
            }
            continue;
        }

        const char *numeric_flags[] = {"-d", "-s", "-f", "-m", "-A", "-c", "-o", "-F", NULL};
        bool found_numeric = false;
        for (int k = 0; numeric_flags[k]; k++) {
            if (!strcmp(arg, numeric_flags[k])) {
                if (i + 1 >= argc || (argv[i+1][0] == '-' && !isdigit((unsigned char)argv[i+1][1]) && argv[i+1][1] != '.')) {
                    print_error_msg(MSG(MSG_ERR_MISSING_VALUE), arg);
                    return 3;
                }
                int res = handle_numeric_argument(arg, argv[++i], opts);
                if (res != 0) return res;
                found_numeric = true;
                break;
            }
        }
        if (found_numeric) continue;

        print_error_msg(MSG(MSG_ERR_INVALID_OPTION), arg);
        show_help();
        return 3;
    }
    return 0;
}

/** Inicializa o ambiente global, carregando localização e configurações do terminal. */
static void init_system_context(void) {
    setlocale(LC_ALL, "");
    msgs_init();
    neonx_init_lut();
    srand((unsigned int)time(NULL));

#ifdef _WIN32
#else
    signal(SIGPIPE, SIG_IGN);
#endif
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
}

/** Trata comandos de exibição de informações (ajuda, versão, licença, integridade). */
static int handle_info_commands(const struct neonx_options *opts)
{
    if (opts->show_help_flag)    { show_help(); return 0; }
    if (opts->show_version_flag) { print_version(); return 0; }
    if (opts->show_license_flag) { print_license(); return 0; }
    
    if (opts->verify_sig_flag) {
        if (auth_status == 0) {
            printf("%s", MSG(MSG_VERIFY_OK));
            return 0;
        }
        if (auth_status == 2) {
            fprintf(stderr, "%s", MSG(MSG_ERR_VERIFY_RESTRICTED));
            return 2;
        }
        fprintf(stderr, "%s", MSG(MSG_VERIFY_FAIL));
        return 1;
    }
    return -1;
}

/** Ferramenta auxiliar para gerar sequências de cores ANSI para scripts externos. */
static void run_spin_tool(void)
{
    const int32_t FIXED_TWO_PI = 0x0006487F, P_G = 0x0002182A, P_B = 0x00043054;
    for (int j = 0; j < 60; j++) {
        int32_t base = (FIXED_TWO_PI * 2 * j) / 60;
        int r = ((neonx_fast_sin_fixed(base) * 127) / FIXED_ONE) + 128;
        int g = ((neonx_fast_sin_fixed(base + P_G) * 127) / FIXED_ONE) + 128;
        int b = ((neonx_fast_sin_fixed(base + P_B) * 127) / FIXED_ONE) + 128;
        printf("38;2;%d;%d;%d ", (r<0?0:(r>255?255:r)), (g<0?0:(g>255?255:g)), (b<0?0:(b>255?255:b)));
    }
    printf("\n");
}

/** Ponto de entrada principal do NeonX. */
int main(int argc, char *argv[]) {
    struct neonx_options opts = {0};
    opts.phase_fixed_set = false;
    opts.stream_mode = false;
    opts.speed_fixed = FLOAT_TO_FIXED(0.2f);
    opts.frame_time_us = 50000;

    init_system_context();
    auth_status = check_integrity();
    set_integrity_status(auth_status);
    
    int parse_res = parse_arguments(argc, argv, &opts);
    if (parse_res != 0) return parse_res;
    
    int info_res = handle_info_commands(&opts);
    if (info_res != -1) return info_res;
    
    if (opts.spin_flag) {
        run_spin_tool();
        return 0;
    }
    
    if (!opts.phase_fixed_set) {
        opts.phase_fixed = neonx_random_phase();
    }

    Content content = {0};
    terminal_setup_signals(&content);

    content.lines = malloc((size_t)g_max_lines_limit * sizeof(wchar_t*));
    if (!content.lines) {
        fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
        return 1;
    }
    memset(content.lines, 0, (size_t)g_max_lines_limit * sizeof(wchar_t*));
    
    load_input_data(&opts, &content);
    
    if (opts.stream_mode) {
        run_stream_mode(&opts, &content);
    } else {
        run_buffered_mode(&opts, &content);
    }
    
    cleanup_and_exit(&content, 0);
    return 0;
}

