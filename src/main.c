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
#ifndef _WIN32
#include <unistd.h>
#endif

#include "integrity.h"
#include "shaders.h"
#include "msgs.h"
#include "render.h"

#ifdef _WIN32
#include <windows.h>
#endif

int auth_status;

// ==================== Funções auxiliares para validação ====================

int g_max_lines_limit = 10000;
extern uint32_t secure_random_u32(void);

static bool is_integer_fixed(int32_t value) {
    return (value & 0xFFFF) == 0;
}

static int32_t str_to_fixed(const char *s) {
    int32_t result = 0;
    int32_t fraction = 0;
    int divisor = 1;
    bool in_fraction = false;
    bool negative = false;
    
    if (*s == '-') {
        negative = true;
        s++;
    }
    
    while (*s) {
        if (*s == '.') {
            in_fraction = true;
        } else if (*s >= '0' && *s <= '9') {
            if (in_fraction) {
                fraction = fraction * 10 + (*s - '0');
                divisor *= 10;
            } else {
                result = result * 10 + (*s - '0');
            }
        }
        s++;
    }
    
    int32_t fixed_val = (result * FIXED_ONE) + ((fraction * FIXED_ONE) / divisor);
    return negative ? -fixed_val : fixed_val;
}

// ==================== Processamento de Argumentos ====================

#ifdef __GNUC__
__attribute__((format(printf, 1, 2)))
#endif
static void print_error_msg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static int handle_numeric_argument(const char *arg, const char *val, struct neonx_options *opts)
{
    int32_t num_fixed = str_to_fixed(val);

    if (!strcmp(arg, "-o")) {
        if (num_fixed < 0 || num_fixed > FIXED_ONE) {
            print_error_msg(MSG(MSG_ERR_INVALID_NUMBER), "-o", val);
            return 3;
        }
        shaders_set_opacity_from_string(val); // Will refactor this function next
        return 0;
    }

    if (!strcmp(arg, "-F")) {
        if (!is_integer_fixed(num_fixed) || num_fixed <= 0) {
            print_error_msg(num_fixed <= 0 ? MSG(MSG_ERR_MUST_BE_POSITIVE) : MSG(MSG_ERR_MUST_BE_INTEGER), "-F");
            return 3;
        }
        opts->frame_time_us = (uint32_t)(1000000LL / (num_fixed / FIXED_ONE));
        return 0;
    }

    if (!strcmp(arg, "-m")) {
        int32_t mode = num_fixed / FIXED_ONE;
        if (!is_integer_fixed(num_fixed) || mode < 0 || mode > MAX_ANIM_MODE) {
            if (mode < 0 || mode > MAX_ANIM_MODE) {
                fprintf(stderr, "%s", MSG(MSG_ERR_MODE_LIMIT));
            } else {
                print_error_msg(MSG(MSG_ERR_MUST_BE_INTEGER), "-m");
            }
            return 4;
        }
        opts->anim_mode = (int)mode;
        return 0;
    }

    if (!strcmp(arg, "-c")) {
        if (!is_integer_fixed(num_fixed) || num_fixed <= 0) {
            print_error_msg(num_fixed <= 0 ? MSG(MSG_ERR_MUST_BE_POSITIVE) : MSG(MSG_ERR_MUST_BE_INTEGER), "-c");
            return 3;
        }
        opts->fixed_width = num_fixed / FIXED_ONE;
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
    else if (!strcmp(arg, "-p")) opts->start_phase_fixed = num_fixed;
    
    return 0;
}

static int parse_arguments(int argc, char *argv[], struct neonx_options *opts) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        
        if (!strcmp(arg, "-max-lines") && i + 1 < argc) {
            g_max_lines_limit = atoi(argv[++i]);
            if (g_max_lines_limit < 100) g_max_lines_limit = 100;
            continue;
        }
        if (!strcmp(arg, "-P") && i + 1 < argc) {
            opts->phase_fixed = str_to_fixed(argv[++i]);
            opts->phase_fixed_set = true;
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
            shaders_set_preset(argv[++i], &opts->anim_mode, &opts->speed_fixed);
            continue;
        }

        const char *numeric_flags[] = {"-d", "-s", "-f", "-m", "-A", "-c", "-o", "-p", "-F", NULL};
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

// ==================== Sub-rotinas do Main ====================

static void init_system_context(void) {
    msgs_init();
    neonx_init_lut();
    srand((unsigned int)time(NULL));
    setlocale(LC_ALL, "");

#ifdef _WIN32
    // Windows não tem SIGPIPE
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

static void run_spin_tool(void)
{
    const int32_t FIXED_TWO_PI = 0x0006487F, P_G = 0x0002182A, P_B = 0x00043054;
    for (int j = 0; j < 60; j++) {
        int32_t base = (FIXED_TWO_PI * 2 * j) / 60;
        int r = ((fast_sin_fixed(base) * 127) / FIXED_ONE) + 128;
        int g = ((fast_sin_fixed(base + P_G) * 127) / FIXED_ONE) + 128;
        int b = ((fast_sin_fixed(base + P_B) * 127) / FIXED_ONE) + 128;
        printf("38;2;%d;%d;%d ", (r<0?0:(r>255?255:r)), (g<0?0:(g>255?255:g)), (b<0?0:(b>255?255:b)));
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    struct neonx_options opts = {0};
    opts.phase_fixed_set = false;
    opts.stream_mode = false;
    opts.speed_fixed = FLOAT_TO_FIXED(0.2f);
    opts.frame_time_us = 50000; // 20 FPS default

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
        opts.phase_fixed = (int32_t)(secure_random_u32() % 65536);
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
