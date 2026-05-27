#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>

#include "integrity.h"
#include "shaders.h"
#include "msgs.h"
#include "render.h"

#ifdef _WIN32
    #include <windows.h>
#endif

int auth_status;

/**
 * Converte string para numero de ponto fixo com seguranca contra overflow.
 */
static int32_t str_to_fixed(const char *arg_name, const char *s) {
    int64_t int_part = 0; 
    int32_t frac_part = 0;
    int frac_len = 0;
    int sign = 1;
    const char *original_s = s;

    if (*s == '-') { sign = -1; s++; }
    
    if (*s == '\0') {
        fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), arg_name, original_s);
        exit(3);
    }

    while (*s != '\0' && *s != '.') {
        if (*s < '0' || *s > '9') {
            fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), arg_name, original_s);
            exit(3);
        }
        int_part = int_part * 10 + (*s - '0');
        if (int_part > 32767) { 
            fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), arg_name, original_s);
            exit(3);
        }
        s++;
    }

    if (*s == '.') {
        s++;
        while (*s != '\0') {
            if (*s < '0' || *s > '9') {
                fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), arg_name, original_s);
                exit(3);
            }
            if (frac_len < 5) {
                frac_part = frac_part * 10 + (*s - '0');
                frac_len++;
            }
            s++;
        }
    }

    while (frac_len < 5) {
        frac_part *= 10;
        frac_len++;
    }

    int32_t frac_fixed = (int32_t)(((int64_t)frac_part * FIXED_ONE) / 100000);
    return (int32_t)(sign * ((int_part * FIXED_ONE) + frac_fixed));
}

/**
 * Analisa os argumentos de linha de comando.
 */
static int parse_arguments(int argc, char *argv[], struct neonx_options *opts) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        
        if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) { opts->show_help_flag = true; continue; }
        if (!strcmp(arg, "-v") || !strcmp(arg, "--version")) { opts->show_version_flag = true; continue; }
        if (!strcmp(arg, "--license")) { opts->show_license_flag = true; continue; }
        if (!strcmp(arg, "--verify-sig")) { opts->verify_sig_flag = true; continue; }
        if (!strcmp(arg, "--spin")) { opts->spin_flag = true; continue; }
        if (!strcmp(arg, "-S")) { opts->static_mode = true; continue; }
        if (!strcmp(arg, "-L")) { opts->stream_mode = true; continue; }
        if (!strcmp(arg, "--quantized")) { use_quantization = true; continue; }
        if (!strcmp(arg, "--lang")) { 
            if (i+1 < argc && argv[i+1][0] != '-') i++; 
            continue; 
        }

        if (!strcmp(arg, "--preset") && i+1 < argc) {
            i++;
            shaders_set_preset(argv[i], &opts->anim_mode, &opts->speed_fixed);
            continue;
        }
        
        bool is_numeric_flag = (!strcmp(arg, "-d") || !strcmp(arg, "-s") || !strcmp(arg, "-f") || 
                                !strcmp(arg, "-m") || !strcmp(arg, "-A") || !strcmp(arg, "-c") || 
                                !strcmp(arg, "-o") || !strcmp(arg, "-p") || !strcmp(arg, "-F"));

        if (is_numeric_flag) {
            if (i + 1 >= argc || argv[i+1][0] == '-') {
                fprintf(stderr, MSG(MSG_ERR_MISSING_VALUE), arg);
                return 3;
            }
            char *val = argv[i+1];
            
            if (!strcmp(arg, "-o")) {
                shaders_set_opacity_from_string(val);
                i++;
                continue;
            }

            int32_t num_fixed = str_to_fixed(arg, val);
            
            if (!strcmp(arg, "-d")) { 
                opts->duration_fixed = num_fixed;
                opts->duration_us = ((uint64_t)opts->duration_fixed * 1000000) / FIXED_ONE;
            } else if (!strcmp(arg, "-s")) {
                opts->speed_fixed = num_fixed;
            } else if (!strcmp(arg, "-f")) { 
                shaders_set_frequency(num_fixed);
            } else if (!strcmp(arg, "-m")) { 
                int tmp = num_fixed / FIXED_ONE;
                if (tmp >= 0 && tmp <= MAX_ANIM_MODE) {
                    opts->anim_mode = tmp; 
                } else { 
                    fprintf(stderr, "%s", MSG(MSG_ERR_MODE_LIMIT)); 
                    return 4; 
                }
            } else if (!strcmp(arg, "-c")) { 
                opts->fixed_width = num_fixed / FIXED_ONE;
            } else if (!strcmp(arg, "-A")) { 
                shaders_set_gradient_angle(num_fixed);
            } else if (!strcmp(arg, "-p")) { 
                opts->start_phase_fixed = num_fixed;
            } else if (!strcmp(arg, "-F")) { 
                if (num_fixed > 0) {
                    int32_t fps_fixed = num_fixed;
                    opts->frame_time_us = (uint32_t)((1000000LL * FIXED_ONE) / fps_fixed);
                }
            }
            i++;
            continue;
        }

        fprintf(stderr, MSG(MSG_ERR_INVALID_OPTION), arg);
        show_help();
        return 3;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_sigint);
    srand((unsigned int)time(NULL));
    
    auth_status = check_integrity();
    set_integrity_status(auth_status);
    setlocale(LC_ALL, "");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    init_lut();
    msgs_init();

    struct neonx_options opts = {0};
    opts.speed_fixed = FLOAT_TO_FIXED(0.2);
    opts.start_phase_fixed = -FIXED_ONE;
    opts.frame_time_us = 50000;

    int parse_res = parse_arguments(argc, argv, &opts);
    if (parse_res != 0) return parse_res;

    if (opts.show_help_flag) { show_help(); return 0; }
    if (opts.show_version_flag) { print_version(); return 0; }
    if (opts.show_license_flag) { print_license(); return 0; }
    
    if (opts.verify_sig_flag) {
        if (auth_status == 0) {
            printf("OK\n");
            return 0;
        } else if (auth_status == 2) {
            fprintf(stderr, "%s", MSG(MSG_ERR_VERIFY_RESTRICTED));
            return 2; 
        } else {
            fprintf(stderr, "FAIL\n");
            return 1;
        }
    }

    if (opts.spin_flag) {
        const int32_t FIXED_TWO_PI = 0x0006487F; 
        const int32_t PHASE_G_OFFSET = 0x0002182A;
        const int32_t PHASE_B_OFFSET = 0x00043054;
        const int CYCLES = 2; 
        for (int j = 0; j < 60; j++) {
            int32_t base_phase = (FIXED_TWO_PI * CYCLES * j) / 60;
            int32_t phase_r = base_phase;
            int32_t phase_g = base_phase + PHASE_G_OFFSET;
            int32_t phase_b = base_phase + PHASE_B_OFFSET;
            int32_t sin_r = fast_sin_fixed(phase_r);
            int32_t sin_g = fast_sin_fixed(phase_g);
            int32_t sin_b = fast_sin_fixed(phase_b);
            int r = ((sin_r * 127) / FIXED_ONE) + 128;
            int g = ((sin_g * 127) / FIXED_ONE) + 128;
            int b = ((sin_b * 127) / FIXED_ONE) + 128;
            if (r < 0) r = 0; if (r > 255) r = 255;
            if (g < 0) g = 0; if (g > 255) g = 255;
            if (b < 0) b = 0; if (b > 255) b = 255;
            printf("38;2;%d;%d;%d ", r, g, b);
        } 
        printf("\n"); 
        return 0;
    }

    shaders_finalize_setup();

    if (opts.start_phase_fixed >= 0) {
        opts.phase_fixed = opts.start_phase_fixed;
    } else {
        const int32_t TWO_PI_FIXED = 411774;
        opts.phase_fixed = (int32_t)(((uint64_t)rand() * TWO_PI_FIXED) / RAND_MAX);
    }
    
    #ifndef _WIN32
    if (isatty(0) && !opts.stream_mode) {
        fprintf(stderr, "%s", MSG(MSG_ERR_SEM_DADOS));
        show_help();
        return 5;
    }
    #endif

    load_input_data(&opts, &content);

    int exit_status = 0;
    if (opts.stream_mode) {
        exit_status = run_stream_mode(&opts, &content);
    } else {
        exit_status = run_buffered_mode(&opts, &content);
    }

    cleanup_and_exit(&content, exit_status);
    return exit_status;
}