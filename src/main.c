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

// Guardião da porta. Onde a resposta criptográfica informará se está puro.
int auth_status;

/**
 * Nome da função: str_to_fixed
 * O que faz: Transforma uma string ("1.25" do comando cli) para numérico em ponto-fixo protegido contra erros.
 * Como funciona: Tenta forçar a conversão usando strtod da biblioteca de C stdlib,
 * verifica vazamentos ou ponteiros sujos após do erro. Restringe os valores máximos e
 * mínimos antes da conversão para inteiros escalados 32 bits de fato.
 * Parâmetros: arg_name (nome que o usário pediu) e s (o valor cru text).
 * Retorno: Inteiro formatado via bitshift. Emite error e sai caso texto seja bizarro ou muito grande.
 * Onde é usada: Em `parse_arguments` para interpretar as config. do usuário.
*/
// ==================== Funções auxiliares para validação ====================

// Verifica se um valor em ponto-fixo é exatamente um inteiro (sem parte fracionária)
static bool is_integer_fixed(int32_t value) {
    return (value % FIXED_ONE) == 0;
}

// Extrai a parte inteira de um fixed-point, verificando se é inteiro.
// Emite erro e sai se não for inteiro.
static int32_t extract_int_fixed(int32_t value, const char *arg_name) {
    if (!is_integer_fixed(value)) {
        fprintf(stderr, MSG(MSG_ERR_MUST_BE_INTEGER), arg_name);
        exit(3);
    }
    return value / FIXED_ONE;
}

// Verifica se o valor fixed-point é estritamente positivo.
// Emite erro e sai se <=0.
static void require_positive_fixed(int32_t value, const char *arg_name) {
    if (value <= 0) {
        fprintf(stderr, MSG(MSG_ERR_MUST_BE_POSITIVE), arg_name);
        exit(3);
    }
}

// Converte string para fixed-point com validação geral.
// (Essa função já existe, mas mantivemos com o nome str_to_fixed)
static int32_t str_to_fixed(const char *arg_name, const char *s) {
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

static int parse_arguments(int argc, char *argv[], struct neonx_options *opts) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        
        // Flags booleanas
        if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))      { opts->show_help_flag = true; continue; }
        if (!strcmp(arg, "-v") || !strcmp(arg, "--version"))   { opts->show_version_flag = true; continue; }
        if (!strcmp(arg, "--license"))                         { opts->show_license_flag = true; continue; }
        if (!strcmp(arg, "--verify-sig"))                      { opts->verify_sig_flag = true; continue; }
        if (!strcmp(arg, "--spin"))                            { opts->spin_flag = true; continue; }
        if (!strcmp(arg, "-S"))                                { opts->static_mode = true; continue; }
        if (!strcmp(arg, "-L"))                                { opts->stream_mode = true; continue; }
        if (!strcmp(arg, "--quantized"))                       { shaders_set_quantization(true); continue; }

        // --lang
        if (!strcmp(arg, "--lang")) {
            if (i+1 < argc && argv[i+1][0] != '-') {
                i++;
                msgs_set_language(argv[i]);
            }
            continue;
        }

        // --preset
        if (!strcmp(arg, "--preset") && i+1 < argc) {
            i++;
            shaders_set_preset(argv[i], &opts->anim_mode, &opts->speed_fixed);
            continue;
        }

        // Verifica se é uma opção que precisa de valor numérico
        int is_numeric = (!strcmp(arg, "-d") || !strcmp(arg, "-s") || !strcmp(arg, "-f") ||
                          !strcmp(arg, "-m") || !strcmp(arg, "-A") || !strcmp(arg, "-c") ||
                          !strcmp(arg, "-o") || !strcmp(arg, "-p") || !strcmp(arg, "-F"));

        if (is_numeric) {
            if (i + 1 >= argc || argv[i+1][0] == '-') {
                fprintf(stderr, MSG(MSG_ERR_MISSING_VALUE), arg);
                return 3;
            }
            char *val = argv[i+1];
            int32_t num_fixed = str_to_fixed(arg, val); // converte string para fixed-point
            i++; // consome o valor

            // -o (opacidade) tem validação própria
            if (!strcmp(arg, "-o")) {
                shaders_set_opacity_from_string(val);
                continue;
            }

            // -F (FPS) – deve ser inteiro positivo
            if (!strcmp(arg, "-F")) {
                if (num_fixed % FIXED_ONE != 0) {
                    fprintf(stderr, MSG(MSG_ERR_MUST_BE_INTEGER), "-F");
                    return 3;
                }
                if (num_fixed <= 0) {
                    fprintf(stderr, MSG(MSG_ERR_MUST_BE_POSITIVE), "-F");
                    return 3;
                }
                int32_t fps = num_fixed / FIXED_ONE;
                opts->frame_time_us = (uint32_t)(1000000LL / fps);
                continue;
            }

            // -m (modo) – deve ser inteiro entre 0 e MAX_ANIM_MODE
            if (!strcmp(arg, "-m")) {
                if (num_fixed % FIXED_ONE != 0) {
                    fprintf(stderr, MSG(MSG_ERR_MUST_BE_INTEGER), "-m");
                    return 4;
                }
                int32_t mode = num_fixed / FIXED_ONE;
                if (mode < 0 || mode > MAX_ANIM_MODE) {
                    fprintf(stderr, "%s", MSG(MSG_ERR_MODE_LIMIT));
                    return 4;
                }
                opts->anim_mode = (int)mode;
                continue;
            }

            // -c (largura fixa) – deve ser inteiro positivo
            if (!strcmp(arg, "-c")) {
                if (num_fixed % FIXED_ONE != 0) {
                    fprintf(stderr, MSG(MSG_ERR_MUST_BE_INTEGER), "-c");
                    return 3;
                }
                int32_t width = num_fixed / FIXED_ONE;
                if (width <= 0) {
                    fprintf(stderr, MSG(MSG_ERR_MUST_BE_POSITIVE), "-c");
                    return 3;
                }
                opts->fixed_width = width;
                continue;
            }

            // -d (duração) – pode ser fracionário, mas não negativo
            if (!strcmp(arg, "-d")) {
                opts->duration_fixed = num_fixed;
                opts->duration_us = ((uint64_t)opts->duration_fixed * 1000000) / FIXED_ONE;
                if (opts->duration_fixed < 0) {
                    fprintf(stderr, "%s", MSG(MSG_ERR_DURATION_NEGATIVE));
                    return 3;
                }
                continue;
            }

            // -s, -f, -A, -p (aceitam fracionários)
            if (!strcmp(arg, "-s")) {
                opts->speed_fixed = num_fixed;
            } else if (!strcmp(arg, "-f")) {
                shaders_set_frequency(num_fixed);
            } else if (!strcmp(arg, "-A")) {
                shaders_set_gradient_angle(num_fixed);
            } else if (!strcmp(arg, "-p")) {
                opts->start_phase_fixed = num_fixed;
            } else {
                // não deveria ocorrer
                fprintf(stderr, MSG(MSG_ERR_INVALID_OPTION), arg);
                return 3;
            }
            continue;
        }

        // Se chegou aqui, argumento não reconhecido
        fprintf(stderr, MSG(MSG_ERR_INVALID_OPTION), arg);
        show_help();
        return 3;
    }
    return 0;
}

/**
 * Nome da função: main
 * O que faz: Ponto de partida padrão (Entry Point) compilador C. Executa toda fita.
 * Como funciona: Instancia variáveis, injeta locale no SO pra aceitar acentos utf8 no prompt, 
 * manda habilitar as flags brutais de render no Windows cmd, e inicia as alocações da matemática pesada em RAM.
 * Parâmetros: argc e *argv de cli inativos.
 * Retorno: Inteiro zero para status OK, ou o código de desligamento de falha.
 * Onde é usada: O próprio Processador/SO do usuário evoca isso ao clicar ou digitar na máquina.
 */
int main(int argc, char *argv[]) {
    // Escuta e aborta por evento do SO. Permite rodar o fechar a cortina antes do abort() padrão matador.
    signal(SIGINT, handle_sigint);
    srand((unsigned int)time(NULL)); // Mistura o cofre randômico
    
    // FIX: Integrity check is informative only – open source allows modifications.
    // Avalia o final do hash para descobrir pirataria crua. Mas, filosoficamente open source, ele avisa
    // pro painel version, ele não trava a lib inteira de funcionar não.
    auth_status = check_integrity();
    set_integrity_status(auth_status);
    
    // Manda OS tentar utilizar pontuações UTF internacionais globais (como vírgulas ao invés de pontos decimais).
    setlocale(LC_ALL, ""); 

// Truques sujos pro ambiente Windows CMD suportar nossas strings de render de escape "\033[38"
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            // ENABLE_VIRTUAL_TERMINAL_PROCESSING é uma var nova oculta do Win10+ que entende nativo ANSI.
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    init_lut(); // Sobe 4096 divisões de senos para a RAM e não gastar cpu mais.
    msgs_init(); // Descobre o Idioma do SO para traduzir todo texto do painel interno.

    // Gera o 'default' (padrão base) caso ninguém mande args por cima.
    struct neonx_options opts = {0};
    opts.speed_fixed = FLOAT_TO_FIXED(0.2);
    opts.start_phase_fixed = -FIXED_ONE;
    opts.frame_time_us = 50000;

    int parse_res = parse_arguments(argc, argv, &opts);
    if (parse_res != 0) return parse_res; // Se preencher as opt explodiu as regras do programador, saia.

    // Ações de ajuda de Terminal que só cospem as coisas rápidas e saem do modo de execução.
    if (opts.show_help_flag) { show_help(); return 0; }
    if (opts.show_version_flag) { print_version(); return 0; }
    if (opts.show_license_flag) { print_license(); return 0; }
    
    // Flag oculta apenas para desenvolvedores de repositório saberem do hash OK ou FAIL do binário.
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

    // Ferramenta que usamos em modo debug só pra gerar os códigos de escape em textos sem grid formatado pra debug
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
            printf("38;2;%d;%d;%d ", r, g, b); // Gera uma fila de RGB strings de forma enxuta e rápida pra pipes simples.
        } 
        printf("\n"); 
        return 0;
    }

    // Configura ângulos e variações trigonométricas complexas predefinidas pelo Arg ou Default
    shaders_finalize_setup();

    // Setup base da semente de aleatoriedade das cores para o frame inicial.
    if (opts.start_phase_fixed >= 0) {
        opts.phase_fixed = opts.start_phase_fixed;
    } else {
        const int32_t TWO_PI_FIXED = 411774;
        opts.phase_fixed = (int32_t)(((uint64_t)rand() * TWO_PI_FIXED) / RAND_MAX); // Aleatoriza cor que se inicia
    }
    
    #ifndef _WIN32
    // Tratativa extra em Linux. Is a TTY flag. Evita usar render gigante sem haver cano de dados atrelado num CMD nu.
    if (isatty(0) && !opts.stream_mode) {
        fprintf(stderr, "%s", MSG(MSG_ERR_SEM_DADOS));
        show_help();
        return 5;
    }
    #endif

    // Preenche nossa struct Content alocando a memória com os textos recebidos no Pipe ("texto_qualquer | neonx")
    load_input_data(&opts, &content);
    set_content_initialized();
    int exit_status = 0;
    
    // Roteador mestre das engrenagens do vídeo vs Log
    if (opts.stream_mode) {
        exit_status = run_stream_mode(&opts, &content);
    } else {
        exit_status = run_buffered_mode(&opts, &content);
    }

    // Executado se o tempo do timeout acabou ou se o loop esgotou em stream/static flags.
    cleanup_and_exit(&content, exit_status);
    return exit_status;
}
