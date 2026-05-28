// ==================== main.c ====================
#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <limits.h>
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

static bool is_integer_fixed(int32_t value)
{
    return (value % FIXED_ONE) == 0;
}

static int32_t str_to_fixed(const char *arg_name, const char *s)
{
    char *endptr;
    double val = strtod(s, &endptr);
    if (*endptr != '\0' || s == endptr) {
        fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), arg_name, s);
        exit(3);
    }
    double max_val = (double)(INT32_MAX) / FIXED_ONE;
    double min_val = (double)(INT32_MIN) / FIXED_ONE;
    if (val > max_val || val < min_val) {
        fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), arg_name, s);
        exit(3);
    }
    return (int32_t)(val * FIXED_ONE);
}

// ==================== Processamento de Argumentos ====================

static int handle_numeric_argument(const char *arg, const char *val, struct neonx_options *opts)
{
    int32_t num_fixed = str_to_fixed(arg, val);

    if (!strcmp(arg, "-o")) {
        shaders_set_opacity_from_string(val);
        return 0;
    }

    if (!strcmp(arg, "-F")) {
        if (!is_integer_fixed(num_fixed) || num_fixed <= 0) {
            fprintf(stderr, MSG(num_fixed <= 0 ? MSG_ERR_MUST_BE_POSITIVE : MSG_ERR_MUST_BE_INTEGER), "-F");
            return 3;
        }
        opts->frame_time_us = (uint32_t)(1000000LL / (num_fixed / FIXED_ONE));
        return 0;
    }

    if (!strcmp(arg, "-m")) {
        int32_t mode = num_fixed / FIXED_ONE;
        if (!is_integer_fixed(num_fixed) || mode < 0 || mode > MAX_ANIM_MODE) {
            fprintf(stderr, "%s", MSG(mode < 0 || mode > MAX_ANIM_MODE ? MSG_ERR_MODE_LIMIT : MSG_ERR_MUST_BE_INTEGER));
            return 4;
        }
        opts->anim_mode = (int)mode;
        return 0;
    }

    if (!strcmp(arg, "-c")) {
        if (!is_integer_fixed(num_fixed) || num_fixed <= 0) {
            fprintf(stderr, MSG(num_fixed <= 0 ? MSG_ERR_MUST_BE_POSITIVE : MSG_ERR_MUST_BE_INTEGER), "-c");
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
    else if (!strcmp(arg, "-f")) shaders_set_frequency(num_fixed);
    else if (!strcmp(arg, "-A")) shaders_set_gradient_angle(num_fixed);
    else if (!strcmp(arg, "-p")) opts->start_phase_fixed = num_fixed;
    
    return 0;
}

static int parse_arguments(int argc, char *argv[], struct neonx_options *opts)
{
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))      { opts->show_help_flag = true; continue; }
        if (!strcmp(arg, "-v") || !strcmp(arg, "--version"))   { opts->show_version_flag = true; continue; }
        if (!strcmp(arg, "--license"))                         { opts->show_license_flag = true; continue; }
        if (!strcmp(arg, "--verify-sig"))                      { opts->verify_sig_flag = true; continue; }
        if (!strcmp(arg, "--spin"))                            { opts->spin_flag = true; continue; }
        if (!strcmp(arg, "-S"))                                { opts->static_mode = true; continue; }
        if (!strcmp(arg, "-L"))                                { opts->stream_mode = true; continue; }
        if (!strcmp(arg, "--quantized"))                       { shaders_set_quantization(true); continue; }

        if (!strcmp(arg, "--lang") && i + 1 < argc) {
            msgs_set_language(argv[++i]);
            continue;
        }

        if (!strcmp(arg, "--preset") && i + 1 < argc) {
            shaders_set_preset(argv[++i], &opts->anim_mode, &opts->speed_fixed);
            continue;
        }

        const char *numeric_flags = "-d-s-f-m-A-c-o-p-F";
        if (strstr(numeric_flags, arg)) {
            if (i + 1 >= argc || argv[i+1][0] == '-') {
                fprintf(stderr, MSG(MSG_ERR_MISSING_VALUE), arg);
                return 3;
            }
            int res = handle_numeric_argument(arg, argv[++i], opts);
            if (res != 0) return res;
            continue;
        }

        fprintf(stderr, MSG(MSG_ERR_INVALID_OPTION), arg);
        show_help();
        return 3;
    }
    return 0;
}

// ==================== Sub-rotinas do Main ====================

static void init_system_context(void)
{
    signal(SIGINT, handle_sigint);
    srand((unsigned int)time(NULL));
    setlocale(LC_ALL, "");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif

    init_lut();
    msgs_init();
}

static int handle_info_commands(const struct neonx_options *opts)
{
    if (opts->show_help_flag)    { show_help(); return 1; }
    if (opts->show_version_flag) { print_version(); return 1; }
    if (opts->show_license_flag) { print_license(); return 1; }
    
    if (opts->verify_sig_flag) {
        if (auth_status == 0) printf("%s", MSG(MSG_VERIFY_OK));
        else if (auth_status == 2) fprintf(stderr, "%s", MSG(MSG_ERR_VERIFY_RESTRICTED));
        else fprintf(stderr, "%s", MSG(MSG_VERIFY_FAIL));
        return 1;
    }
    return 0;
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

// ==================== Entry Point ====================

int main(int argc, char *argv[])
{
    auth_status = check_integrity();
    set_integrity_status(auth_status);
    init_system_context();

    struct neonx_options opts = {
        .speed_fixed = FLOAT_TO_FIXED(0.2),
        .start_phase_fixed = -FIXED_ONE,
        .frame_time_us = 50000
    };

    int parse_res = parse_arguments(argc, argv, &opts);
    if (parse_res != 0) return parse_res;

    if (handle_info_commands(&opts)) return 0;
    if (opts.spin_flag) { run_spin_tool(); return 0; }

    shaders_finalize_setup();
    opts.phase_fixed = (opts.start_phase_fixed >= 0) ? opts.start_phase_fixed : 
                       (int32_t)(((uint64_t)rand() * 411774) / RAND_MAX);

#ifndef _WIN32
    if (isatty(0) && !opts.stream_mode) {
        fprintf(stderr, "%s", MSG(MSG_ERR_SEM_DADOS));
        show_help();
        return 5;
    }
#endif

    load_input_data(&opts, &content);
    set_content_initialized();
    
    int exit_status = opts.stream_mode ? run_stream_mode(&opts, &content) : 
                                         run_buffered_mode(&opts, &content);

    cleanup_and_exit(&content, exit_status);
    return exit_status;
}
