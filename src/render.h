// ==================== render.h ====================
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <stdbool.h>
#include "terminal.h"

// Estrutura que guarda todas as dezenas de opções (features) que o usuário solicitou via Linha de Comando (CLI).
// Isso agrupa os booleanos permitindo ser facilmente passada em 1 único ponteiro pelas funções no programa.
struct neonx_options {
    bool static_mode;
    bool stream_mode;
    int anim_mode;
    int32_t speed_fixed;
    int32_t duration_fixed;
    uint64_t duration_us;
    int32_t start_phase_fixed;
    int32_t phase_fixed;
    bool phase_fixed_set;
    int fixed_width;
    uint32_t frame_time_us;
    bool show_help_flag;
    bool show_version_flag;
    bool show_license_flag;
    bool verify_sig_flag;
    bool spin_flag;
};

// Funções públicas expostas de rendering
void load_input_data(struct neonx_options *opts, Content *content_ptr);
int run_stream_mode(struct neonx_options *opts, Content *content_ptr);
int run_buffered_mode(struct neonx_options *opts, Content *content_ptr);
void cleanup_and_exit(Content *content_ptr, int exit_code);

#endif