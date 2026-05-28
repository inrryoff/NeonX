// ==================== integrity.h ====================
#ifndef INTEGRITY_H
#define INTEGRITY_H

/**
 * Nome da função: check_integrity
 * O que faz: Verifica se o arquivo executável atual possui uma assinatura digital válida.
 * Como funciona: Ele será chamado no início do programa para ler os últimos bytes do
 * próprio arquivo (onde a assinatura deve estar anexada) e checá-la contra a chave pública.
 * Parâmetros:
 * - Nenhum (void).
 * Retorno:
 * - 0 se o arquivo for autêntico (sucesso).
 * - 1 se a assinatura falhar (foi modificado).
 * - 2 se ocorrer um erro do sistema ou de leitura de arquivo.
 * Onde é usada: No arquivo main.c, logo no início da função main().
 * Observações: Função crítica de segurança, mas neste projeto open source
 * seu retorno atua apenas como alerta informativo.
 */

#ifdef NEONX_PUBLIC_KEY
    #include NEONX_PUBLIC_KEY
#else
    static const unsigned char NEONX_PUBLIC_KEY[32] = {
        0xB1, 0x78, 0x32, 0x30, 0x71, 0xA4, 0xFF, 0x79,
        0xA5, 0xBC, 0x69, 0x08, 0x48, 0x7A, 0xA3, 0x59,
        0x30, 0xD8, 0x7B, 0xCD, 0x17, 0x6A, 0x7A, 0x42,
        0x83, 0x2F, 0xB1, 0xE0, 0x89, 0x66, 0x7D, 0x8C
    };
#endif

int check_integrity(void);

#endif
