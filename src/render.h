#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <stdbool.h>
#include "terminal.h"

/** Configurações globais de execução do NeonX. */
struct neonx_options {
    bool static_mode;
    bool stream_mode;
    int anim_mode;
    int32_t speed_fixed;
    int32_t duration_fixed;
    uint64_t duration_us;
    int32_t phase_fixed;
    bool phase_fixed_set;
    int fixed_width;
    uint32_t frame_time_us;
    bool show_help_flag;
    bool show_version_flag;
    bool show_license_flag;
    bool verify_sig_flag;
    bool spin_flag;
    int c1_r, c1_g, c1_b;
    int c2_r, c2_g, c2_b;
    bool c1_set, c2_set;
};

/** Carrega os dados da entrada para renderização. */
void load_input_data(struct neonx_options *opts, Content *content_ptr);

/** Executa o processamento em modo streaming. */
int run_stream_mode(struct neonx_options *opts, Content *content_ptr);

/** Executa a animação em modo bufferizado. */
int run_buffered_mode(struct neonx_options *opts, Content *content_ptr);

/** Finaliza o programa e restaura o terminal. */
void cleanup_and_exit(Content *content_ptr, int exit_code);

#endif