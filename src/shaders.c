// ==================== shaders.c ====================
#include "shaders.h"
#include "msgs.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Variáveis globais encapsuladas apenas neste arquivo (static)
static int32_t freq_fixed = FLOAT_TO_FIXED(0.3f);     // Frequência do gradiente 
static int32_t opacity_fixed = 0;                     // Grau de escurecimento nas bordas
static int32_t gradient_angle_fixed = FLOAT_TO_FIXED(-1.0f); // Ângulo para animação diagonal
static bool use_quantization = false;                 // Modo retrô de cor (8-bit)
static int32_t sin_lut_fixed[LUT_SIZE];               // Tabela de lookup (memória) para seno

// Guardam o cálculo antecipado do seno e cosseno do ângulo do gradiente
static int32_t grad_cos_fixed = 0;
static int32_t grad_sin_fixed = 0;

// Constante para converter radianos diretamente num índice da nossa tabela LUT
// Matemáticamente: (LUT_SIZE / (2 * PI)) * FIXED_ONE
#define RAD_TO_INDEX_FIXED 42722831L

/**
 * Nome da função: init_lut
 * O que faz: Inicializa uma Look Up Table (LUT) - que é como uma "tábua de resultados" - com valores da função seno.
 * Como funciona: A função seno (usada p/ transições de cor suaves) é custosa para o processador calcular centenas
 * de milhares de vezes por segundo na tela. Nós calculamos 4096 respostas possíveis uma única vez aqui no início, 
 * e as salvamos na RAM (`sin_lut_fixed`). Durante o desenho da tela, é só "consultar a tábua".
 * Parâmetros: Nenhum.
 * Retorno: Nenhum. Preenche o array `sin_lut_fixed`.
 * Onde é usada: No main() logo que o programa inicia.
 * Observações: O resultado armazenado já está na escala Fixed-Point (multiplicado por 65536).
 */
void init_lut(void) { 
    for(int i = 0; i < LUT_SIZE; i++) {
        // sin() retorna de -1.0 a 1.0. FLOAT_TO_FIXED escala isso para a nossa matemática de inteiros.
        sin_lut_fixed[i] = FLOAT_TO_FIXED(sin(2.0 * M_PI * i / LUT_SIZE));
    }
}

/**
 * Nome da função: precalc_gradient_angle
 * O que faz: Calcula antecipadamente as componentes trigonométricas se o usuário solicitar que o gradiente gire num ângulo.
 * Como funciona: Extrai o seno e cosseno do ângulo desejado. Isso impede recálculos matemáticos repetitivos.
 * Parâmetros: Nenhum.
 * Retorno: Nenhum.
 * Onde é usada: Na função shaders_finalize_setup.
 * Observações: Só executa trabalho se gradient_angle_fixed for diferente do valor desativado (-1).
 */
void precalc_gradient_angle(void) {
    if (gradient_angle_fixed >= 0) {
        // Converte de Graus para Radianos, pois a função do C (cosf e sinf) só entende radianos
        float rad = FIXED_TO_FLOAT(gradient_angle_fixed) * M_PI / 180.0f;
        grad_cos_fixed = FLOAT_TO_FIXED(cosf(rad));
        grad_sin_fixed = FLOAT_TO_FIXED(sinf(rad));
    }
}

/**
 * Nome da função: isqrt64
 * O que faz: Calcula a raiz quadrada de um número inteiro de 64 bits de forma extremamente rápida sem usar ponto flutuante.
 * Como funciona: Usa um algoritmo puramente baseado em deslocamento de bits (bit shifting) e aproximação bit-a-bit.
 * Procura do bit mais significativo ao menos significativo ajustando o valor 'root' até convergir.
 * Parâmetros:
 * - n: O número do qual queremos a raiz quadrada.
 * Retorno: A raiz quadrada inteira calculada (uint32_t).
 * Onde é usada: Pela função fast_dist_fixed para calcular distância pitagórica na tela.
 * Observações: Evita gargalos (stutters) garantindo fluidez, já que não chama sqrt() padrão do sistema.
 */
uint32_t isqrt64(uint64_t n) {
    uint32_t root = 0;
    uint64_t bit = 1ULL << 62; // Começa pelo bit mais alto possível para 64-bits

    // Procura o bit onde a raiz pode começar
    while (bit > n) bit >>= 2;    
    
    // Algoritmo clássico de extração de raiz bit a bit
    while (bit != 0) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return root;
}

/**
 * Nome da função: fast_dist_fixed
 * O que faz: Calcula a distância 2D na tela usando o Teorema de Pitágoras (Distância = √(dx² + dy²)).
 * Como funciona: Remove a escala fixed-point dos deltas antes do quadrado, calcula a soma 
 * dos quadrados (que é um número altíssimo e por isso requer 64-bits `int64_t`),
 * faz a raiz quadrada rápida com `isqrt64`, e depois devolve a escala fixed-point `<< 16`.
 * Parâmetros:
 * - dx: Diferença no eixo X (em formato fixed-point).
 * - dy: Diferença no eixo Y (em formato fixed-point).
 * Retorno: A distância linear real daquele pixel até o centro (em formato fixed-point).
 * Onde é usada: No cálculo de opacidade das bordas, animações de Pulso, etc.
 * Observações: Crucial para o desempenho em renderizações em tela cheia.
 */
int32_t fast_dist_fixed(int32_t dx, int32_t dy) {
    int64_t dx_normal = dx >> FIXED_SHIFT; // Tira a escala (ex: 65536 vira 1)
    int64_t dy_normal = dy >> FIXED_SHIFT;
    uint64_t sq = (dx_normal * dx_normal) + (dy_normal * dy_normal);
    return (int32_t)(isqrt64(sq) << FIXED_SHIFT); // Aplica a escala novamente no fim
}

/**
 * Nome da função: fast_sin_fixed
 * O que faz: Extrai o valor do seno diretamente da nossa tábua 'LUT' criada lá em `init_lut`.
 * Como funciona: Converte o valor X no sistema fixed-point para um índice de 0 a 4095.
 * Descobre em qual índice o valor está mais próximo (val1) e qual é o próximo índice (val2).
 * Aplica Interpolação Linear entre eles para suavizar o visual e mascarar as quebras da tabela.
 * Parâmetros:
 * - x_fixed: O grau contínuo de oscilação do momento (fase), em fixed-point.
 * Retorno: O valor seno aproximado correspondente (em fixed-point, entre -65536 e 65536).
 * Onde é usada: Basicamente no cerne de todos os cálculos de cores de todos os efeitos.
 * Observações: Muito superior a chamar sin() da <math.h> em loops aninhados.
 */
int32_t fast_sin_fixed(int32_t x_fixed) {
    // Escala x para o tamanho da LUT
    int32_t scaled = FIXED_MUL(x_fixed, RAD_TO_INDEX_FIXED);
    
    // bitwise AND (&) com LUT_SIZE-1 funciona como "Módulo 4096" (% 4096), mas super rápido
    int idx = ((uint32_t)scaled >> FIXED_SHIFT) & (LUT_SIZE - 1);
    int next = (idx + 1) & (LUT_SIZE - 1);
    
    // Tratamento de segurança (fallback) para evitar acesso inválido à memória
    if (idx < 0 || idx >= LUT_SIZE) idx = 0;
    if (next < 0 || next >= LUT_SIZE) next = 0;

    // Fração decimal para fazer a interpolação (suavização entre 2 pontos)
    int32_t frac = scaled & (FIXED_ONE - 1);
    
    // Pega o valor base e o valor do próximo ponto da tábua
    int32_t val1 = sin_lut_fixed[idx];
    int32_t val2 = sin_lut_fixed[next];
    
    // Equação da reta: Valor Inicial + (Fração * Diferença_Para_O_Prox_Valor)
    return val1 + FIXED_MUL(frac, (val2 - val1));
}

/**
 * Nome da função: shader_sunset_fixed
 * O que faz: Calcula a fase de um shader estilo "pôr do sol" (anim mode 1).
 * Como funciona: Cria duas ondas que viajam em velocidades ligeiramente diferentes nos eixos X e Y
 * e soma os senos delas.
 * Parâmetros: x e y (posições na tela) e phase (fase do tempo atual do programa).
 * Retorno: Valor do padrão calculado em fixed-point para dar cor ao pixel.
 * Onde é usada: Chamada pela função mestre get_color_fast() se modo == 1.
 */
int32_t shader_sunset_fixed(int32_t x, int32_t y, int32_t phase) {
    int32_t p1 = FIXED_MUL(x, FLOAT_TO_FIXED(0.15f)) + phase;
    int32_t p2 = FIXED_MUL(y, FLOAT_TO_FIXED(0.15f)) + (phase >> 1); // >> 1 faz ser 50% da fase (mais lento)
    return fast_sin_fixed(p1) + fast_sin_fixed(p2);
}

/**
 * Nome da função: pseudo_rand
 * O que faz: Gera um número "aleatório" falso mas determinístico com base em coordenadas.
 * Como funciona: Mistura bits de x, y e da fase de tempo usando números primos grandes e a técnica XOR.
 * Sempre que essa função receber exatamente as mesmas entradas, retornará o mesmo número.
 * Parâmetros: Posicionamentos X, Y, e Fase temporal.
 * Retorno: Inteiro simulando aleatoriedade de 31-bits limpos.
 * Onde é usada: Efeitos como o Shader "Matrix" para desenhar ruído (faíscas/sparkles) na tela.
 * Observações: Inline significa que o compilador é instruído a jogar esse código pra dentro da 
 * função principal que chamá-lo pra salvar ciclos de CPU.
 */
static inline int32_t pseudo_rand(int32_t x, int32_t y, int32_t phase) {
    uint32_t state = (uint32_t)x * 1103515245U;
    state += (uint32_t)y * 123456789U;
    state += (uint32_t)phase * 987654321U;
    state = state ^ (state >> 16);
    return (int32_t)(state & 0x7FFFFFFF); // Garante que o retorno seja sempre positivo (tira o bit de sinal)
}

/**
 * Nome da função: shader_matrix_fixed
 * O que faz: Gera uma simulação do código caindo da Matrix (anim mode 10).
 * Como funciona: 
 * 1. Cria a base de varredura diagonal.
 * 2. Gera um 'pulse' de intensidade pulsante de brilho baseado no tempo.
 * 3. Identifica as colunas para o 'scanline' (linhas mais escuras caindo).
 * 4. Gera 'sparkles' pseudoaleatórios que piscam na tela.
 * 5. Multiplica e controla a intensidade de saída através de um ponteiro (*intensity).
 * Parâmetros:
 * - x, y, phase (como nos outros shaders).
 * - *intensity: Um ponteiro! A função modifica a variável de intensidade de quem a chamou.
 * Retorno: A fase do padrão em fixed-point para calcular as cores.
 * Onde é usada: Na get_color_fast() se modo == 10.
 */
int32_t shader_matrix_fixed(int32_t x, int32_t y, int32_t phase, int32_t *intensity) {
    int32_t p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.1f)) + FIXED_MUL(y, FLOAT_TO_FIXED(0.1f));
    
    // O Brilho pulsa rapidamente.
    int32_t pulse = FIXED_MUL(fast_sin_fixed(FIXED_MUL(phase, 3)), FLOAT_TO_FIXED(0.15f)) + FLOAT_TO_FIXED(0.85f);
    
    // A varredura calcula blocos de 10 linhas. Se a linha está no final do bloco, fica mais clara.
    int32_t y_int = y >> FIXED_SHIFT;
    int32_t phase_int = (FIXED_MUL(phase, 5)) >> FIXED_SHIFT;
    int32_t y_mod_int = (y_int - phase_int) % 10;
    if (y_mod_int < 0) y_mod_int += 10;
    
    int32_t scanline = (y_mod_int < 1) ? FLOAT_TO_FIXED(0.7f) : FIXED_ONE;
    
    // Simula as letras brancas nas extremidades da matrix
    int32_t sparkle = (pseudo_rand(x, y, phase) % 100 > 98) ? FLOAT_TO_FIXED(0.5f) : FIXED_ONE;
    
    // A intensidade final é a junção dos 3 efeitos de brilho
    *intensity = FIXED_MUL(FIXED_MUL(pulse, scanline), sparkle);
    if (*intensity < FLOAT_TO_FIXED(0.15f)) *intensity = FLOAT_TO_FIXED(0.15f); // Não deixa a tela apagar 100%
    return p;
}

/**
 * Nome da função: shader_pulse_fixed
 * O que faz: Simula um efeito de pulsar circular que emana do centro da tela (anim mode 11).
 * Como funciona: Identifica a distância do X/Y atual até o centro (CX/CY). Baseado nessa
 * distância versus o tempo da Fase, altera a intensidade do pixel.
 * Parâmetros: Coordenadas atuais e centro (cx, cy), Fase de tempo e ponteiro de intensidade.
 * Retorno: A fase padrão da animação.
 * Onde é usada: Na get_color_fast() se modo == 11.
 */
int32_t shader_pulse_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t phase, int32_t *intensity) {
    int32_t dx = x - cx;
    int32_t dy = y - cy;
    int32_t dist = fast_dist_fixed(dx, dy);    
    int32_t p = dist - phase;
    
    // Cria ondas circulares de energia usando o seno da distância
    int32_t sin_val = fast_sin_fixed((dist >> 1) - (phase << 1));
    
    // Normaliza o resultado entre 0 e 1, aplicando decaimento base de iluminação (0.2 a 1.0)
    *intensity = (sin_val + FIXED_ONE) >> 1; 
    *intensity = FIXED_MUL(*intensity, FLOAT_TO_FIXED(0.8f)) + FLOAT_TO_FIXED(0.2f);
    return p;
}

/**
 * Nome da função: apply_border_opacity_fixed
 * O que faz: Escurece suavemente o texto próximo às extremidades, gerando um efeito de "Vinheta".
 * Como funciona: Calcula a distância do pixel pro meio. Verifica a proporção em relação à 
 * distância máxima possível (canto da tela). O parâmetro 'op' (opacidade) dita qual é 
 * a força desse fator 'decay'. No final multiplica diretamente nos canais R, G e B do pixel.
 * Parâmetros:
 * - x, y: Posição do pixel desenhado agora.
 * - cx, cy: Centro da tela/texto renderizado.
 * - max_dist: A distância entre o centro e as bordas.
 * - op: Grau de opacidade de borda aplicado (de 0 a 1000).
 * - r, g, b: Ponteiros para a cor já calculada anteriormente, aqui ela será "escurecida".
 * Retorno: Nenhum. Modifica as variáveis de cor apontadas pelos ponteiros r, g, b.
 * Onde é usada: Na get_color_fast, caso o modo selecionado sofra alteração ambiental.
 */
void apply_border_opacity_fixed(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t max_dist, int32_t op, int *r, int *g, int *b) {
    if (op <= 0) return; // Otimização rápida
    if (max_dist == 0) return;
    
    int32_t dx = x - cx;
    int32_t dy = y - cy;
    int32_t dist = fast_dist_fixed(dx, dy);
    
    // Tira os números do sistema Fixed-Point pra usar na escala % (1000)
    int32_t max_d_normal = max_dist >> FIXED_SHIFT;
    int32_t dist_normal = dist >> FIXED_SHIFT;
    
    if (max_d_normal == 0) return;
    
    // Regra de três: Se DistMax = 1000, DistAtual = Quanto? (dist_ratio)
    int32_t dist_ratio = (dist_normal * 1000) / max_d_normal;
    int32_t decay = (dist_ratio * op) / 1000;
    int32_t factor = 1000 - decay;
    
    if (factor < 0) factor = 0;
    if (factor > 1000) factor = 1000;
    
    // Aplica o fator de escurecimento sobre as cores cruas (R,G,B)
    *r = (int)(((int64_t)*r * factor) / 1000);
    *g = (int)(((int64_t)*g * factor) / 1000);
    *b = (int)(((int64_t)*b * factor) / 1000);
    
    // Evita glitches matemáticos (clamp)
    if (*r < 0) *r = 0; else if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; else if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; else if (*b > 255) *b = 255;
}

/**
 * Nome da função: get_color_fast
 * O que faz: O cérebro do renderizador. Retorna a cor final Vermelha, Verde e Azul de 1 caractere na tela.
 * Como funciona: 
 * 1. Entra no "switch(mode)" escolhendo qual padrão visual o usuário pediu no comando (e.g. -m 4).
 * 2. Executa a matemática exigida por esse padrão para descobrir a "fase p" desse pixel específico.
 * 3. Multiplica essa fase pela frequência geral escolhida (velocidade de cores).
 * 4. Alimenta essa fase nas ondas seno de R, G e B, defasadas em ~120 graus cada 
 * (Isso é como um prisma girando, gera todas as cores do arco íris RGB suavemente).
 * 5. Aplica restrições extras solicitadas (Quantização retrô ou Vinheta Escurecida).
 * Parâmetros: Inúmeros detalhes contextuais do momento da tela + Ponteiros p/ as variáveis RGB do Render.
 * Retorno: Nenhum, sobrescreve por ponteiros os ints *r, *g, *b.
 * Onde é usada: Dentro da função mestre `print_colored_line` que escreve linha por linha na tela.
 */
void get_color_fast(int32_t x, int32_t y, int mode, int32_t cx, int32_t cy, int32_t max_dist, int32_t phase, int *r, int *g, int *b) {
    int32_t p;
    int32_t intensity = FIXED_ONE; // Por padrão, a intensidade de luz é 100%
    
    // Bloco gigantesco de decisão. Cada case é uma animação/visual diferente.
    switch(mode) {  
        case 1: p = shader_sunset_fixed(x, y, phase); break;
        case 2: p = phase; break; // Cor sólida que respira/muda junto com o tempo
        case 3: p = (y >> 1) + phase; break; // Listras puramente horizontais caindo
        case 4: p = FIXED_MUL(y, FLOAT_TO_FIXED(0.8f)) + FIXED_MUL(fast_sin_fixed(FIXED_MUL(x, FLOAT_TO_FIXED(0.3f))), FLOAT_TO_FIXED(3.0f)) + phase; break; // Curvas de onda clássicas retrô
        case 5: p = fast_dist_fixed(x - cx, y - cy) - phase; break;  // Radiação do centro pura
        case 6: p = FIXED_MUL(FIXED_MUL(fast_sin_fixed(FIXED_MUL(x, FLOAT_TO_FIXED(0.2f))), fast_sin_fixed(FIXED_MUL(y, FLOAT_TO_FIXED(0.2f)) + FIXED_PI_2)), FLOAT_TO_FIXED(10.0f)) + phase; break;  // Efeito caleidoscópio complexo
        case 7: { 
            // O modo 7 escurece os lados horizontais
            p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.1f)); 
            int32_t diff = x > cx ? x - cx : cx - x;
            int32_t ratio = (cx == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cx);
            intensity = FIXED_ONE - FIXED_MUL(ratio, FLOAT_TO_FIXED(0.8f)); 
            if(intensity < FLOAT_TO_FIXED(0.2f)) intensity = FLOAT_TO_FIXED(0.2f); 
            break;
        }  
        case 8: { 
            // O modo 8 escurece o topo e base (vertical)
            p = phase + FIXED_MUL(y, FLOAT_TO_FIXED(0.2f)); 
            int32_t diff = y > cy ? y - cy : cy - y;
            int32_t ratio = (cy == 0) ? 0 : (int32_t)(((int64_t)diff * FIXED_ONE) / cy);
            intensity = FIXED_ONE - FIXED_MUL(ratio, FLOAT_TO_FIXED(0.7f)); 
            if(intensity < FLOAT_TO_FIXED(0.3f)) intensity = FLOAT_TO_FIXED(0.3f); 
            break;
        }  
        case 9: p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.1f)); intensity = ((x >> FIXED_SHIFT) % 2 == 0) ? FIXED_ONE : FLOAT_TO_FIXED(0.6f); break; // Zebrado
        case 10: p = shader_matrix_fixed(x, y, phase, &intensity); break;  
        case 11: p = shader_pulse_fixed(x, y, cx, cy, phase, &intensity); break;
        default: 
            // Modo 0 (Padrão/Cyberpunk de Gradiente Diagonal Clássico)
            p = phase + FIXED_MUL(x, FLOAT_TO_FIXED(0.2f)) + FIXED_MUL(y, FLOAT_TO_FIXED(0.1f)); 
            // Adiciona a influência do ângulo definido pelo usuário (se foi escolhido no CLI)
            if (gradient_angle_fixed >= 0) {
                p += FIXED_MUL(grad_cos_fixed, x) + FIXED_MUL(grad_sin_fixed, y); 
            } 
            intensity = FIXED_ONE;
            break;
    }
    
    // A Fase Base representa onde o pixel atual está na viagem do tempo vs cor
    int32_t base_phase = FIXED_MUL(freq_fixed, p);
    
    // Geração da Paleta RGB via defasagem trigonométrica.
    // Estão espaçados em 120 Graus (2.094 Radianos pro Verde e 4.188 pro Azul)
    int32_t sin_r = fast_sin_fixed(base_phase);
    int32_t sin_g = fast_sin_fixed(base_phase + FLOAT_TO_FIXED(2.094f));
    int32_t sin_b = fast_sin_fixed(base_phase + FLOAT_TO_FIXED(4.188f));
    
    // 'raw_r' é normalizado da escala senoidal (-65536~65536) para o valor padrão de tela sRGB (0~255)
    int raw_r = ((sin_r * 127) >> FIXED_SHIFT) + 128;
    int raw_g = ((sin_g * 127) >> FIXED_SHIFT) + 128;
    int raw_b = ((sin_b * 127) >> FIXED_SHIFT) + 128;
    
    // O modo zero não é influenciado pela intensidade, então atalho direto.
    if (mode == 0) {
        *r = raw_r; *g = raw_g; *b = raw_b;
    } else {
        *r = (raw_r * intensity) >> FIXED_SHIFT;
        *g = (raw_g * intensity) >> FIXED_SHIFT;
        *b = (raw_b * intensity) >> FIXED_SHIFT;
    }

    // Se o usuário passou --quantized, nós zeramos os 3 últimos bits de cada cor (ex: 11111111 -> 11111000)
    // Isso cria "Banding", um visual serrilhado de cores dos anos 90 onde as transições são bruscas.
    if (use_quantization) {
        *r &= 0xF8; *g &= 0xF8; *b &= 0xF8;
    }
    
    // Garante segurança na memória: Cores devem existir somente no escopo de 1 byte (0-255)
    if (*r < 0) *r = 0; if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; if (*b > 255) *b = 255;
    
    if (mode != 0) {
        apply_border_opacity_fixed(x, y, cx, cy, max_dist, opacity_fixed, r, g, b);
    }
}

/**
 * Nome da função: shaders_set_preset
 * O que faz: Aplica configurações combinadas e otimizadas pra simular uma atmosfera "Pronta".
 * Como funciona: Identifica a string (cyberpunk, retro, matrix, sunset) e sobrescreve as vars globais.
 * Parâmetros: preset(nome), anim_mode(ponteiro p/ sobrescrever), speed_fixed(ponteiro).
 * Retorno: Nenhum.
 * Onde é usada: Na função parse_arguments, se detectou a flag --preset no CLI.
 */
void shaders_set_preset(const char *preset, int *anim_mode, int32_t *speed_fixed) {
    if (!strcmp(preset,"cyberpunk")) { 
        *anim_mode = 0; *speed_fixed = FLOAT_TO_FIXED(0.3); freq_fixed = FLOAT_TO_FIXED(0.5); gradient_angle_fixed = FLOAT_TO_FIXED(45.0);
    } else if (!strcmp(preset,"retro")) { 
        *anim_mode = 4; *speed_fixed = FLOAT_TO_FIXED(0.2); freq_fixed = FLOAT_TO_FIXED(0.8); gradient_angle_fixed = FLOAT_TO_FIXED(0.0);
    } else if (!strcmp(preset,"matrix")) { 
        *anim_mode = 10; *speed_fixed = FLOAT_TO_FIXED(0.5); freq_fixed = FLOAT_TO_FIXED(1.2); gradient_angle_fixed = FLOAT_TO_FIXED(90.0);
    } else if (!strcmp(preset,"sunset")) { 
        *anim_mode = 1; *speed_fixed = FLOAT_TO_FIXED(0.15); freq_fixed = FLOAT_TO_FIXED(0.3); gradient_angle_fixed = FLOAT_TO_FIXED(30.0);
    }
}

// ---- GETTERS/SETTERS PÚBLICOS ENCAPSULANDO O ARQUIVO ----- //
/**
 * Nome da função: shaders_set_frequency
 * O que faz: Altera a distância/largura do arco-íris de transição (frequência).
 */
void shaders_set_frequency(int32_t freq) {
    freq_fixed = freq;
}

/**
 * Nome da função: shaders_set_gradient_angle
 * O que faz: Altera a inclinação das linhas de cor (de 0 a 360 Graus).
 */
void shaders_set_gradient_angle(int32_t angle) {
    gradient_angle_fixed = angle;
}

/**
 * Nome da função: shaders_set_quantization
 * O que faz: Ativa ou desativa o efeito de cor retrô "pixelado".
 */
void shaders_set_quantization(bool enable) {
    use_quantization = enable;
}

/**
 * Nome da função: shaders_set_opacity_from_string
 * O que faz: O usuário escreve "-o 0.8" no terminal e ela converte isso para valor interno e seguro de Opacidade (800).
 * Como funciona: Usa strtod para segurança com limite restrito entre 0 e 1.
 */
void shaders_set_opacity_from_string(const char *o_val) {
    char *endptr;
    double val = strtod(o_val, &endptr);
    
    // Se converteu errado, ou se o número é menor que zero ou maior que 1... Saída violenta de erro
    if (*endptr != '\0' || o_val == endptr || val < 0.0 || val > 1.0) {
        fprintf(stderr, MSG(MSG_ERR_INVALID_NUMBER), "-o", o_val);
        exit(3); // Sai do programa acusando erro sintático 3
    }
    
    // Transforma algo como "0.80" num int puro "800".
    opacity_fixed = (int32_t)(val * 1000.0);
}

/**
 * Nome da função: shaders_finalize_setup
 * O que faz: Função designada para ser chamada após processar todos os comandos da inicialização do usuário.
 * Onde é usada: Na main().
 */
void shaders_finalize_setup(void) {
    precalc_gradient_angle();
}