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
#include "msgs.h"
#include "render.h"
#include "neonx.h"

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

/** Converte uma cor em formato hexadecimal (ex: #FF0000) para RGB. */
static bool parse_hex_color(const char *hex, int *r, int *g, int *b) {
    if (!hex) return false;
    if (hex[0] == '#') hex++;
    size_t len = strlen(hex);
    if (len != 6) return false;
    
    for (size_t i = 0; i < 6; i++) {
        if (!isxdigit((unsigned char)hex[i])) return false;
    }

    unsigned int val;
    if (sscanf(hex, "%x", &val) != 1) return false;
    
    *r = (val >> 16) & 0xFF;
    *g = (val >> 8) & 0xFF;
    *b = val & 0xFF;
    return true;
}

/** Exibe mensagens de erro formatadas no fluxo de erro padrão (stderr). */
static void print_error_msg(const char *msg, const char *arg1, const char *arg2) {
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    if (arg2) fprintf(stderr, msg, arg1, arg2);
    else if (arg1) fprintf(stderr, msg, arg1);
    else fprintf(stderr, "%s", msg);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
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
        opts->vertical_opacity = false;
        neonx_set_vertical_opacity(false);
        neonx_set_opacity(num_fixed);
        return 0;
    }

    if (!strcmp(arg, "-O")) {
        if (num_fixed < 0 || num_fixed > FIXED_ONE) {
            print_error_msg(MSG(MSG_ERR_INVALID_NUMBER), "-O", val);
            return 3;
        }
        opts->vertical_opacity = true;
        neonx_set_vertical_opacity(true);
        neonx_set_opacity(num_fixed);
        return 0;
    }

    if (!strcmp(arg, "-F")) {
        int32_t fps = num_fixed / FIXED_ONE;
        if (fps <= 0) {
            print_error_msg(MSG(MSG_ERR_MUST_BE_POSITIVE), "-F", NULL);
            return 3;
        }
        opts->frame_time_us = (uint32_t)(1000000LL / fps);
        return 0;
    }

    if (!strcmp(arg, "-m")) {
        int32_t mode = num_fixed / FIXED_ONE;
        if (mode < 0 || mode > 11) {
            fprintf(stderr, "%s", MSG(MSG_ERR_MODE_LIMIT));
            return 4;
        }
        opts->anim_mode = (int)mode;
        return 0;
    }

    if (!strcmp(arg, "-c")) {
        int32_t width = num_fixed / FIXED_ONE;
        if (width <= 0) {
            print_error_msg(MSG(MSG_ERR_MUST_BE_POSITIVE), "-c", NULL);
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

    if (!strcmp(arg, "-s")) {
        opts->speed_fixed = num_fixed;
        opts->speed_set = true;
    } else if (!strcmp(arg, "-f")) {
        opts->freq_fixed = num_fixed;
        opts->freq_set = true;
    } else if (!strcmp(arg, "-A")) {
        opts->angle_fixed = num_fixed;
        opts->angle_set = true;
    }
    
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
        if (!strcmp(arg, "--no-ansi")) { opts->disable_ansi = true; continue; }
        if (!strcmp(arg, "--fo")) {
            opts->matte_mode = true;
            neonx_set_matte_mode(true);
            /* Verifica se o próximo argumento é uma intensidade numérica */
            if (i + 1 < argc && (isdigit((unsigned char)argv[i+1][0]) || (argv[i+1][0] == '.' && isdigit((unsigned char)argv[i+1][1])))) {
                bool ok;
                int32_t intensity = secure_str_to_fixed(argv[++i], &ok);
                if (ok && intensity >= 0 && intensity <= FIXED_ONE) {
                    neonx_set_matte_intensity(intensity);
                }
            }
            continue;
        }
        
        if (!strcmp(arg, "--lang") && i + 1 < argc) {
            msgs_set_language(argv[++i]);
            continue;
        }

        if ((!strcmp(arg, "--color1") || !strcmp(arg, "--c1")) && i + 1 < argc) {
            int r, g, b;
            if (parse_hex_color(argv[++i], &r, &g, &b)) {
                opts->c1_r = r; opts->c1_g = g; opts->c1_b = b;
                opts->c1_set = true;
            } else {
                print_error_msg(MSG(MSG_ERR_INVALID_OPTION), argv[i], NULL);
                return 3;
            }
            continue;
        }
        if ((!strcmp(arg, "--color2") || !strcmp(arg, "--c2")) && i + 1 < argc) {
            int r, g, b;
            if (parse_hex_color(argv[++i], &r, &g, &b)) {
                opts->c2_r = r; opts->c2_g = g; opts->c2_b = b;
                opts->c2_set = true;
            } else {
                print_error_msg(MSG(MSG_ERR_INVALID_OPTION), argv[i], NULL);
                return 3;
            }
            continue;
        }

        if (!strcmp(arg, "--preset") && i + 1 < argc) {
            const char *preset_val = argv[++i];
            if (preset_val[0] == '-') {
                print_error_msg(MSG(MSG_ERR_MISSING_VALUE), arg, NULL);
                return 3;
            }
            if (!shaders_set_preset(preset_val, opts)) {
                print_error_msg(MSG(MSG_ERR_INVALID_OPTION), preset_val, NULL);
                return 3;
            }
            continue;
        }

        const char *numeric_flags[] = {"-d", "-s", "-f", "-m", "-A", "-c", "-o", "-F", "-O", NULL};
        bool found_numeric = false;
        for (int k = 0; numeric_flags[k]; k++) {
            if (!strcmp(arg, numeric_flags[k])) {
                if (i + 1 >= argc || (argv[i+1][0] == '-' && !isdigit((unsigned char)argv[i+1][1]) && argv[i+1][1] != '.')) {
                    print_error_msg(MSG(MSG_ERR_MISSING_VALUE), arg, NULL);
                    return 3;
                }
                int res = handle_numeric_argument(arg, argv[++i], opts);
                if (res != 0) return res;
                found_numeric = true;
                break;
            }
        }
        if (found_numeric) continue;

        print_error_msg(MSG(MSG_ERR_INVALID_OPTION), arg, NULL);
        show_help(opts->disable_ansi);
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
    if (opts->show_help_flag)    { show_help(opts->disable_ansi); return 0; }
    if (opts->show_version_flag) { print_version(opts->disable_ansi); return 0; }
    if (opts->show_license_flag) { print_license(opts->disable_ansi); return 0; }
    
    if (opts->verify_sig_flag) {
        if (auth_status == 0) {
            printf("%s", MSG(MSG_VERIFY_OK));
            return 0;
        }
        if (auth_status == 2) {
            fprintf(stderr, "%s", MSG(MSG_ERR_INTEGRITY_FAIL));
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
    init_system_context();
    auth_status = check_integrity();
    set_integrity_status(auth_status);

    struct neonx_options opts = {0};
    opts.speed_fixed = FLOAT_TO_FIXED(0.2f);
    opts.freq_fixed = 19660; // 0.3 default
    opts.angle_fixed = -65536; // -1 default
    opts.frame_time_us = 16666; // 60 FPS default

    int parse_res = parse_arguments(argc, argv, &opts);
    if (parse_res != 0) return parse_res;

    /* Aplica parâmetros finais ao motor */
    neonx_set_frequency(opts.freq_fixed);
    neonx_set_gradient_angle(opts.angle_fixed);

    if (opts.c1_set && opts.c2_set) {
        neonx_set_custom_gradient(opts.c1_r, opts.c1_g, opts.c1_b, opts.c2_r, opts.c2_g, opts.c2_b);
    }
    
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
    content.line_lens = malloc((size_t)g_max_lines_limit * sizeof(size_t));
    if (!content.lines || !content.line_lens) {
        fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
        if (content.lines) free(content.lines);
        if (content.line_lens) free(content.line_lens);
        return 1;
    }
    memset(content.lines, 0, (size_t)g_max_lines_limit * sizeof(wchar_t*));
    memset(content.line_lens, 0, (size_t)g_max_lines_limit * sizeof(size_t));
    
    load_input_data(&opts, &content);
    
    int res;
    if (opts.stream_mode) {
        res = run_stream_mode(&opts, &content);
    } else {
        res = run_buffered_mode(&opts, &content);
    }
    cleanup_and_exit(&content, res);
    return res;
}
