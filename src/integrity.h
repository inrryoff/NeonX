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
int check_integrity(void);

#endif
