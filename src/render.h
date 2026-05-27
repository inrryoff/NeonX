// ==================== render.h ====================
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <stdbool.h>
#include "terminal.h"

// Estrutura que guarda todas as dezenas de opções (features) que o usuário solicitou via Linha de Comando (CLI).
// Isso agrupa os booleanos permitindo ser facilmente passada em 1 único ponteiro pelas funções no programa.
struct neonx_options {
    bool static_mode;          // Se true, ele gera cor para o texto, printa e sai, não faz vídeo (animação)
    bool stream_mode;          // Modo Log contínuo sem limites (-L)
    int anim_mode;             // Número do Modo visual (0 a 11)
    int32_t speed_fixed;       // Velocidade de alteração com o tempo
    int32_t duration_fixed;    
    uint64_t duration_us;      // Se definido, mata o programa automaticamente após X milissegundos rodando (-d)
    int32_t start_phase_fixed; // Seed congelada (-p) de cor se quisermos gerar saídas repetíveis
    int32_t phase_fixed;       // Variável viva da fase de pulso do tempo atual
    int fixed_width;           // Engana a matemática fingindo que a tela é de tal tamanho (-c)
    uint32_t frame_time_us;    // Pausa do sleep pra controlar os quadros (-F, ex FPS 60 = 16000ms)
    
    // Flags de utilidades simples da inicialização
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