// ==================== msgs.h ====================
#ifndef MSGS_H
#define MSGS_H

/**
 * Enum: Mensagem
 * O que faz: Cria um "catálogo" de identificadores únicos para cada texto do programa.
 * Como funciona: No C, um 'enum' atribui números inteiros automaticamente (0, 1, 2...)
 * para cada item da lista. Em vez de decorar que o erro de arquivo é a mensagem número 0,
 * o programador apenas escreve MSG_ERRO_ABRIR.
 * Observações: O último item, MSG_TOTAL, é um truque clássico da linguagem C.
 * Como ele é o último da lista, o número dele representará exatamente a quantidade
 * total de mensagens cadastradas, o que é útil para criar o tamanho da matriz (array).
 */

enum Mensagem {
    MSG_ERRO_ABRIR,
    
    // Versão
    MSG_VERSION_ORIGINAL_CREATOR,
    MSG_VERSION_COMPILED_BY,
    MSG_VERSION_STATUS_OFFICIAL,
    MSG_VERSION_STATUS_MODIFIED,
    MSG_VERSION_STATUS_ERROR,

    // Licença
    MSG_LICENSE_TEXT,

    // Ajuda
    MSG_HELP_HEADER,
    MSG_HELP_USAGE,
    MSG_HELP_M,
    MSG_HELP_S,
    MSG_HELP_F,
    MSG_HELP_D,
    MSG_HELP_A,
    MSG_HELP_P,
    MSG_HELP_S_UPPER,
    MSG_HELP_C,
    MSG_HELP_O,
    MSG_HELP_F_UPPER,
    MSG_HELP_L,
    MSG_HELP_PRESET,
    MSG_HELP_QUANTIZED,
    MSG_HELP_SPIN,
    MSG_HELP_LANG,
    MSG_HELP_LICENSE,
    MSG_HELP_VERSION,
    MSG_HELP_HELP,

    // Erros
    MSG_ERR_MISSING_VALUE,
    MSG_ERR_INVALID_NUMBER,
    MSG_ERR_MODE,
    MSG_ERR_MODE_LIMIT,
    MSG_ERR_INVALID_OPTION,
    MSG_ERR_SEM_DADOS,
    MSG_ERR_LEN_LIMIT,
    MSG_ERR_VERIFY_RESTRICTED,
    MSG_ERR_INTEGRITY_OPEN,
    MSG_ERR_INTEGRITY_READ,
    MSG_ERR_INTEGRITY_MEMORY,
    MSG_ERR_INTEGRITY_SIGNATURE,
    MSG_ERR_INTEGRITY_SIZE,
    MSG_ERR_MUST_BE_INTEGER,
    MSG_ERR_MUST_BE_POSITIVE,
    MSG_ERR_DURATION_NEGATIVE,
    MSG_ERR_MEMORY_ALLOC,

    MSG_TOTAL 
};
/**
 * Nome da função: get_msg
 * O que faz: Busca o texto exato na memória correspondente ao ID da mensagem e ao idioma atual.
 * Parâmetros:
 * - id: O identificador da mensagem definido no 'enum Mensagem' acima.
 * Retorno: Um ponteiro de texto constante (const char*) contendo a frase traduzida.
 */
const char* get_msg(enum Mensagem id);

/**
 * Macro: MSG
 * O que faz: Cria um atalho prático para os desenvolvedores.
 * Como funciona: Sempre que o programador escrever `MSG(id)` no código, o compilador 
 * substitui automaticamente por `get_msg(id)` antes de compilar. 
 * Por que foi feito: Reduz a digitação, deixando o código fonte (como os printfs) mais limpo e legível.
 */
#define MSG(id) get_msg(id)

/**
 * Nome da função: msgs_init
 * O que faz: Descobre qual é a língua atual do Sistema Operacional do usuário para setar o idioma do NeonX.
 */
void msgs_init(void);
void msgs_set_language(const char *lang_code);
#endif