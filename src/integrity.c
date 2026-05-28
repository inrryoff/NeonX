// ==================== integrity.c ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "msgs.h"
#include "integrity.h"
#include "monocypher.h" // Biblioteca de criptografia externa

// Configuração de diretórios específicos de acordo com o sistema operacional
#ifdef _WIN32
#include <windows.h>
#ifndef MAX_PATH
#define MAX_PATH 260 // Limite padrão de caminhos no Windows
#endif
#elif defined(__APPLE__)
#include <mach-o/dyld.h> // Necessário para acessar caminho do executável no macOS
#else
#include <unistd.h>      // Necessário para acessar o sistema de arquivos no Linux/Unix
#endif

// Chave pública (32 bytes) gravada diretamente no código-fonte (hardcoded).
// É usada para validar a assinatura anexada no final do binário executável.

static unsigned char active_public_key[32];
static bool key_loaded = false;

/**
 * Nome da função: load_active_key
 * Tenta carregar a chave de keys/NeonX.pub ou da variável NEONX_KEY_FILE.
 * Se falhar, usa a chave embutida (NEONX_PUBLIC_KEY).
 */
static void load_active_key(void)
{
    if (key_loaded) return;

    const char *env_path = getenv("NEONX_KEY_FILE");
    const char *path = env_path ? env_path : "keys/NeonX.pub";

    FILE *f = fopen(path, "rb");
    if (f) {
        if (fread(active_public_key, 1, 32, f) == 32) {
            fclose(f);
            key_loaded = true;
            return;
        }
        fclose(f);
    }

    // Se o usuário especificou um caminho via ENV e falhou, avisa usando o dicionário.
    if (env_path) {
        fprintf(stderr, "%s", MSG(MSG_WARN_KEY_LOAD_FAIL));
    }

    // Fallback
    memcpy(active_public_key, NEONX_PUBLIC_KEY, 32);
    key_loaded = true;
}

/**
 * Nome da função: check_integrity
 * O que faz: Checa a integridade do próprio arquivo executável.
 * Como funciona:
 * 1. Descobre o caminho no disco para o executável atual.
 * 2. Abre o arquivo em modo leitura binária.
 * 3. Lê os últimos 128 bytes (que contêm a assinatura em formato hexadecimal).
 * 4. Converte os 128 caracteres hexadecimais para 64 bytes binários reais.
 * 5. Lê o restante do arquivo (o programa original sem a assinatura).
 * 6. Usa a biblioteca Monocypher (EdDSA) para checar se a assinatura confere com o arquivo.
 * Parâmetros:
 * - Nenhum.
 * Retorno: 0 (Sucesso), 1 (Falha de Assinatura), 2 (Erro do Sistema/Arquivo).
 * Onde é usada: main() no arquivo main.c.
 * Observações: Aloca memória para armazenar o executável durante a checagem,
 * portanto pode exigir um espaço razoável de RAM equivalente ao tamanho do programa.
 */
int check_integrity(void)
{
    char path[1024]; // Buffer para armazenar o caminho do executável

    // O processo de descobrir o caminho do próprio executável muda em cada sistema operacional:
#ifdef _WIN32
    // Pega o caminho no Windows
    if (GetModuleFileNameA(NULL, path, sizeof(path)) == 0) return 2;
#elif defined(__APPLE__)
    // Pega o caminho no macOS
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) return 2;
#else
    // Pega o caminho no Linux lendo o link simbólico /proc/self/exe
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len <= 0) {
        return 2; // Falha na leitura
    }
    path[len] = '\0'; // Garante que a string termine corretamente
#endif

    // Abre o arquivo do próprio executável no modo 'rb' (read binary)
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "%s", MSG(MSG_ERR_INTEGRITY_OPEN));
        return 2;
    }
    // Move o ponteiro de leitura para o final do arquivo
    fseek(f, 0, SEEK_END);
    long total_size = ftell(f); // Onde o ponteiro está = tamanho total do arquivo

    // Se o arquivo for menor que a assinatura (128 bytes), é impossível ser válido
    if (total_size <= 128) {
        fclose(f);
        fprintf(stderr, "%s", MSG(MSG_ERR_INTEGRITY_SIZE));
        return 1;
    }

    // Move o ponteiro para ler especificamente os últimos 128 bytes
    fseek(f, total_size - 128, SEEK_SET);
    char sig_hex[129] = {0}; // Buffer para os 128 caracteres da assinatura + \0
    if (fread(sig_hex, 1, 128, f) != 128) {
        fclose(f);
        return 2; // Falha na leitura do disco
    }

    // Converte os caracteres Hexadecimais ("FF") em bytes de verdade (0xFF)
    unsigned char signature[64];
    for (int i = 0; i < 64; i++) {
        unsigned int byte;
        // sscanf lê de 2 em 2 caracteres hex (%2x) e salva no inteiro 'byte'
        if (sscanf(sig_hex + i*2, "%2x", &byte) != 1) {
            fclose(f);
            return 1; // Se a formatação for inválida, falhou
        }
        signature[i] = (unsigned char)byte;
    }

    // Calcula o tamanho do programa SEM a assinatura no final
    size_t data_len = total_size - 128;

    // Aloca memória RAM para carregar todo o arquivo para validação
    unsigned char *data = malloc(data_len);
    if (!data) {
        fclose(f);
        fprintf(stderr, "%s", MSG(MSG_ERR_INTEGRITY_MEMORY));
        return 2;
    }

    // Volta o ponteiro do arquivo para o início e lê o programa inteiro para a RAM
    fseek(f, 0, SEEK_SET);
    size_t bytes_read = fread(data, 1, data_len, f);
    fclose(f); // Arquivo lido, já podemos fechar o acesso ao disco

    if (bytes_read != data_len) {
        free(data);
        return 2; // O tamanho lido não bate com o esperado
    }

    // Carrega a chave (arquivo, env ou fallback)
    load_active_key();

    // A função crypto_eddsa_check (da Monocypher) faz a matemática pesada
    // validando os dados com a chave pública. Resulta em 0 se tudo for autêntico.
    int result = crypto_eddsa_check(signature, active_public_key, data, data_len);

    free(data); // Libera a memória RAM alocada
    return (result == 0) ? 0 : 1;
}
