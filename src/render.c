// ==================== render.c ====================
#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include "render.h"
#include "shaders.h"
#include "msgs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <unistd.h>
#include <stdarg.h>

#ifdef _WIN32
    #include <windows.h>
    #ifndef STDOUT_FILENO
        #define STDOUT_FILENO 1
    #endif
    #define write _write 
#endif

#ifdef _WIN32
    #define wcsdup _wcsdup
#endif

/**
 * Nome da função: get_time_us
 * O que faz: Devolve um número absoluto de tempo que o computador está vivo desde o boot, em precisão microssegundo.
 * Como funciona: Chamada de API sensível do Windows vs. CLOCK_MONOTONIC do Unix.
 * Parâmetros: Nenhum.
 * Retorno: O instante de tempo decorrido no universo da placa mãe. (uint64_t gigantesco).
 * Onde é usada: Na run_buffered_mode, para descobrir se estourou o tempo -d de encerramento do script.
 */
static uint64_t get_time_us(void) {
#ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER now;
    QueryPerformanceFrequency(&freq); // Descobre a resolução fina de tempo do processador.
    QueryPerformanceCounter(&now);
    return (uint64_t)((now.QuadPart * 1000000LL) / freq.QuadPart);
#else
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts); // Monotonic significa que o tempo não volta se a hr da bios mudar
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
#endif
}

/**
 * Nome da função: cleanup_and_exit
 * O que faz: Executa o desligamento limpo do programa para não zoar a tela do usuário com bugs.
 * Como funciona: Dá um reset explícito (via write ANSI) reativando quebra de linha `\033[?7h`, ativando 
 * visibilidade do cursor piscante original `\033[?25h`, e limpa cores ativas `\033[0m`. Limpa memória final.
 * Parâmetros: ponteiro para memória e o status code final do programa.
 * Retorno: Morte do processo (exit() do SO).
 * Onde é usada: Acontece no finalzinho do main().
 */
void cleanup_and_exit(Content *content_ptr, int exit_code) {
    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16);
    free_content(content_ptr);
    exit(exit_code);
}

/**
 * Nome da função: load_input_data
 * O que faz: Lê os dados em formato puro passados no final do "pipe" de bash (Ex: `ls | neonx`).
 * Como funciona: Extrai tudo que vem do terminal do SO `stdin` (Standard Input), armazena num array de strings (wstring 
 * para dar suporte a caracteres complexos utf-8), até que encontre um limite ou falha.
 * Parâmetros: ponteiro para opções e para o objeto base de conteúdo.
 * Retorno: Nenhum. Carrega o texto pro ponteiro diretamente.
 * Onde é usada: No fluxo principal do main().
 * Observações: Transforma Escapes maliciosos (0x1B) em interrogações para evitar a quebra da matemática ANSI.
 */
void load_input_data(struct neonx_options *opts, Content *content_ptr) {
    if (opts->stream_mode) return; // Em modo infinito/logs não pré-lêmos, processamos vivo.

    wchar_t buf[MAX_LINE_LEN];
    content_ptr->count = 0;

    // Repete a leitura via loop enquanto ainda houver texto entrando
    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        if (content_ptr->count >= (MAX_LINES - 1)) {
            // Se as linhas excederem o tamanho limite máximo predefinido da RAM...
            fprintf(stderr, "%s", MSG(MSG_ERR_LEN_LIMIT));
            sleep_us(2000000); 
            // Vira o app para funcionar num loop log sem limite em vez de bugar de vez
            opts->stream_mode = true;
            break; 
        }

        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0'; // Tira o enter/newline falso da leitura
        for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; } // Bloqueio ANSI explícito de segurança
        
        // Copia seguramente o buffer (que vai sobrescrever) para um espaço novo alocado p/ a struct com wcsdup.
        wchar_t *dup_buf = wcsdup(buf);
        if (!dup_buf) {
            fprintf(stderr, "%s", MSG(MSG_ERR_MEMORY_ALLOC));
            exit(6);
        }
        
        content_ptr->lines[content_ptr->count++] = dup_buf;
    }
}

/**
 * Nome da função: safe_append
 * O que faz: Ferramenta de buffer que anexa (append) string complexa ao final de um monte de strings.
 * Como funciona: Usa a função do C que permite enviar parâmetros dinâmicos infinitos '...', converte-os usando
 * vsnprintf já protegendo o tamanho contra estouros de limite (buffer overflow).
 * Parâmetros:
 * - ptr: Ponteiro na memória do meio da fila para começar a escrever.
 * - rem: Quantidade de bytes máxima que faltam na capacidade antes de explodir.
 * - fmt: Variáveis com coringas igual no printf (%d, %s...).
 * Retorno: A quantidade de bytes (comprimento de string) que foram injetados com êxito (size_t).
 * Onde é usada: Exclusivamente em run_buffered_mode pela sub-função `print_colored_line` que chamá-la.
 */
static size_t safe_append(char *ptr, size_t rem, const char *fmt, ...) {
    if (rem == 0) return 0;
    
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(ptr, rem, fmt, args);
    va_end(args);
    
    if (n < 0) return 0;   // Erro na formatação
    
    if ((size_t)n >= rem) {
        // Truncamento: escreve o máximo possível e garante terminador nulo
        // A última posição já é '\0' pelo vsnprintf quando n >= rem
        return rem - 1;     // Quantos bytes foram escritos (excluindo o \0 final)
    }
    
    return (size_t)n;
}

/**
 * Nome da função: print_colored_line
 * O que faz: Analisa letra a letra da linha e "envolve" com o comando invisível da cor embutida em ANSI ESCAPE.
 * Como funciona: Pula cálculos vazios como o espaço ' ' ou tab. Descobre o RGB de cada letra no grid, 
 * e se for uma cor diferente da letra anterior, envia pra memória a tag de alteração ANSI ("\033[38;2;R;G;Bm").
 * Parâmetros: Diversos parâmetros da fase do shader, e os ponteiros pra modificar as filhas (buf e rem).
 * Retorno: Nenhum, sobrescreve externamente os buffers.
 * Onde é usada: Compartilhada tanto no 'modo buffer' gigante quanto no 'modo streaming' linha a linha.
 * Observações: Esse truque de só enviar a cor de novo quando `last_r` for diferente, otimiza DRASTICAMENTE
 * a velocidade e taxa de quadros (FPS) pois economiza gigantes bytes por varredura.
 */
static void print_colored_line(wchar_t *line, int32_t y_fixed, int32_t phase, struct neonx_options *opts, int32_t cx_fixed, int32_t cy_fixed, int32_t max_dist_fixed, char **buf_ptr, size_t *rem_ptr) {
    int line_len = wcslen(line);
    int last_r = -1, last_g = -1, last_b = -1; // -1 porque as cores vão de 0 a 255. Ou seja, inicializa virgem.
    
    for (int x = 0; x < line_len; x++) {
        int r, g, b;
        int32_t x_fixed = FLOAT_TO_FIXED(x);
        
        // Pede para o motor mágico do arquivo shaders lhe devolver o RGB daquele pixel exato.
        get_color_fast(x_fixed, y_fixed, opts->anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
        
        if (line[x] == L' ' || line[x] == L'\t') { // Se não tem pixels pra preencher visualmente na fonte (um espaço)
            if (buf_ptr && rem_ptr) { // Tá em modo buffer gigante? Append nele
                size_t w = safe_append(*buf_ptr, *rem_ptr, "%lc", line[x]);
                *buf_ptr += w; *rem_ptr -= w;
            } else { // Tá no modo stream vivo de logs? Printa de vez na tela agora!
                printf("%lc", line[x]);
            }
        } else {
            // Aqui tem pixel vivo pra pintar de fato. Mudou da última cor pintada pra poupar memória?
            if (r != last_r || g != last_g || b != last_b) {
                if (buf_ptr && rem_ptr) {
                    // String complexa mágica ESC (\033[) 38 = texto; 2 = RGB verdadeiro (16 milhoes); cores; e 'm'.
                    size_t w = safe_append(*buf_ptr, *rem_ptr, "\033[38;2;%d;%d;%dm", r, g, b);
                    *buf_ptr += w; *rem_ptr -= w;
                } else {
                    printf("\033[38;2;%d;%d;%dm", r, g, b);
                }
                last_r = r; last_g = g; last_b = b; // Atualiza a cor anterior para a atual lida
            }
            if (buf_ptr && rem_ptr) {
                size_t w = safe_append(*buf_ptr, *rem_ptr, "%lc", line[x]);
                *buf_ptr += w; *rem_ptr -= w;
            } else {
                printf("%lc", line[x]);
            }
        }
    }
}

/**
 * Nome da função: run_stream_mode
 * O que faz: Modo usado pelo aplicativo quando precisamos analisar logs em tempo real sem limite de tamanho (-L).
 * Como funciona: Diferente do `buffered`, ele não re-sobrescreve toda a tela apagando o anterior, ele processa
 * uma cor animada em uma linha e as deixa ir embora, rolando pra baixo eternamente.
 * Parâmetros: ponteiros para estrutura e contexto.
 * Retorno: Inteiro que emite 1 se o erro fechar violentamente o terminal STDIN.
 * Onde é usada: Chamada do main.c.
 */
int run_stream_mode(struct neonx_options *opts, Content *content_ptr) {
    int line_count = content_ptr->count;
    write(STDOUT_FILENO, "\033[?7l\033[?25l", 11); // Esconde temporariamente o cursor '_' feio do DOS.
    int exit_status = 0;
    int32_t phase = opts->phase_fixed;

    // Loop inicial limpando as linhas que foram cacheadas em memória RAM antes da ativação ser decretada.
    for (int i = 0; i < line_count; i++) {
        wchar_t *line_buf = content_ptr->lines[i];
        size_t len = wcslen(line_buf);
        int32_t y_fixed = FLOAT_TO_FIXED(i);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f); 
        int32_t cx_fixed = FLOAT_TO_FIXED(len / 2.0f);
        int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
        
        // Executa coloração no momento e empurra com enter.
        print_colored_line(line_buf, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, NULL, NULL);
        
        printf("\033[0m\n"); // Tira a tinta do pincel pra garantir limpeza
        fflush(stdout); // Força a placa enviar pra tela na hr!
        phase += opts->speed_fixed;
        free(content_ptr->lines[i]); // Linha morta, destrói!
    }
    content_ptr->count = 0; // Zera a lousa inteira.

    // Entra num Loop Infinito de captura real-time de tudo que o terminal injeta pra nós em stdin.
    wchar_t buf[MAX_LINE_LEN];
    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; }
        
        int32_t y_fixed = FLOAT_TO_FIXED(line_count);
        int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
        int32_t cx_fixed = FLOAT_TO_FIXED(len / 2.0f);
        int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
        
        print_colored_line(buf, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, NULL, NULL);
        
        printf("\033[0m\n");
        fflush(stdout); // Força impressão real time
        phase += opts->speed_fixed;
        line_count++;
        // Se a fonte principal (quem chamou o pipe de nós) morreu ou fechou, saia daqui com erro amigável.
        if (ferror(stdin)) {
            exit_status = 1;
            break;
        }
    }
    return exit_status;
}

/**
 * Nome da função: run_buffered_mode
 * O que faz: Modo principal e bonito de funcionamento! Um loop que joga tinta na tela com a matemática fluida alterando.
 * Como funciona: Pega um tamanho enorme de memória, descobre como um desenho ficará nela cor-a-cor, formata as 
 * strings completas da matriz de linhas do SO inteira. Manda TUDO numa pancada só pra placa. Dá a ordem 
 * ANSI de voltar o ponteiro do cursor pra linha 0 no teto da tela e pisca e substitui de novo no prox quadro.
 * Parâmetros: ponteiros para estrutura e contexto lido na RAM.
 * Retorno: Inteiro zero se sair OK, ou código numérico do problema.
 * Onde é usada: Na main().
 */
int run_buffered_mode(struct neonx_options *opts, Content *content_ptr) {
    if (content_ptr->count == 0) return 0; // O arquivo estava vazio.
    
    // Calcula quem é a linha mais longa do arquivo pra podermos padronizar o centro (cx) perfeito.
    int max_w = 0;
    for(int i = 0; i < content_ptr->count; i++) { 
        int l = wcslen(content_ptr->lines[i]); 
        if(l > max_w) max_w = l;
    }
    if (opts->fixed_width > 0) max_w = opts->fixed_width;
    
    // Aloca memória brutal equivalente as letras da linha VS metadado de tintas (aprox 32 char extras de metadado).
    size_t buf_size = content_ptr->count * (max_w * 32 + 100);
    char *frame_buf = malloc(buf_size);
    if (!frame_buf) {
        return 6; // Error out of memory
    }
    
    int32_t phase = opts->phase_fixed; // Configura o tempo (t0)
    write(STDOUT_FILENO, "\033[?25l\033[?7l", 11); // Esconde cursor '_' da animação
    bool first_frame = true;
    
    // Centro do documento de texto como um quadro (Canvas) em formato Decimal (x=0.5, y=0.5)
    int32_t cx_fixed = FLOAT_TO_FIXED(max_w / 2.0f);
    int32_t cy_fixed = FLOAT_TO_FIXED(content_ptr->count / 2.0f);
    int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
    uint64_t start_time_us = get_time_us();

    // LOOP INFINITO DE ANIMAÇÃO DA TELA MAGICA (VÍDEO DE FATO)
    while (1) {
        // Encerramento se existir temporizador prévia
        if (opts->duration_us > 0) {
            uint64_t now_us = get_time_us();
            if (now_us - start_time_us >= opts->duration_us) break;
        }
        
        char *ptr = frame_buf; // Reposiciona ponteiro no frame '0' do frame_buf todo quadro.
        size_t rem = buf_size; // Reseta capacidade da RAM
        size_t w;
        
        // Sequência ANSI secreta que previne "Tearing" ou flickering (Pisco indesejado em renderizadores velhos).
        // Isso comanda os terminais compativeis a segurarem o buffer atualizando todos grid pixels de 1 vez só.
        w = safe_append(ptr, rem, "\033[?2026h"); ptr += w; rem -= w;
        
        if (!first_frame) { 
            // Se já foi renderizado uma vez, pede pro SO subir com o 'cursor fantasma' todas as linhas de volta ao início!
            if (content_ptr->count > 100) {
                w = safe_append(ptr, rem, "\033[H"); ptr += w; rem -= w; // Vai pro teto (1,1) se for mt gigante
            } else {
                w = safe_append(ptr, rem, "\033[%dA", content_ptr->count); ptr += w; rem -= w; // Sobe exatas X linhas
            }
        }
        first_frame = false;
        
        // Varredura y/x injetando os pixels do quadro dentro do Buffer da string do ponteiro ptr.
        for (int y = 0; y < content_ptr->count; y++) {
            wchar_t *line = content_ptr->lines[y];
            int32_t y_fixed = FLOAT_TO_FIXED(y);
            
            w = safe_append(ptr, rem, "\r"); ptr += w; rem -= w; // Carriage return limpo
            
            // Re-utilizando a maestria otimizada pra processar as cores e jogar na nossa RAM do 'ptr'
            print_colored_line(line, y_fixed, phase, opts, cx_fixed, cy_fixed, max_dist_fixed, &ptr, &rem);
            
            // \033[K deleta tudo pra direita da linha impedindo "lixo" que sobrou na tela sujar essa!
            w = safe_append(ptr, rem, "\033[0m\033[K\n"); ptr += w; rem -= w;
        }
        
        // Fecha/Confirma pra placa de video soltar o buffer congelado de anti-tearing
        w = safe_append(ptr, rem, "\033[?2026l"); ptr += w; rem -= w;
        
        // ESCRITA DEFINITIVA DO FRAME DO SEGUNDO DE TINTA NO TERMINAL!! O milagre acontece.
        write(STDOUT_FILENO, frame_buf, ptr - frame_buf);
        
        // Comandos extras (quebra a timeline de animação se fomos chamados com o param -S p/ estático)
        if (opts->static_mode) break;
        
        // Incrementa o valor "p" global pros pixels re-re-calcularem outras cores com um pequeno delay.
        phase += opts->speed_fixed;
        sleep_us(opts->frame_time_us); // Pausa do processador para não drenar energia absurda da máquina inteira à toa.
    }

    free(frame_buf); // Libera o bloco gigaton alocado ali no começo
    return 0;
}