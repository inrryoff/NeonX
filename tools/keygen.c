#include <stdio.h>
#include <stdlib.h>
#include "../src/monocypher.h"

int main() {
    uint8_t seed[32];         // 32 bytes aleatórios
    uint8_t secret_key[64];   // Monocypher usa 64 bytes para chave secreta
    uint8_t public_key[32];   // chave pública

    // 1. Gerar semente aleatória
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) {
        perror("Erro ao abrir /dev/urandom");
        return 1;
    }
    fread(seed, 1, 32, f);
    fclose(f);

    // 2. Construir chave secreta (seed nos primeiros 32 bytes)
    memcpy(secret_key, seed, 32);

    // 3. Derivar par de chaves (preenche secret_key[32..63] com a pública e ajusta)
    crypto_eddsa_key_pair(secret_key, public_key, secret_key);

    // 4. Salvar chave privada (os 64 bytes)
    f = fopen("priv.key", "wb");
    if (f) {
        fwrite(secret_key, 1, 64, f);
        fclose(f);
        printf("Chave privada salva em priv.key\n");
    } else {
        printf("Aviso: não foi possível salvar priv.key\n");
    }

    // 5. Exibir chave pública em hex
    printf("Chave pública (hex): ");
    for (int i = 0; i < 32; i++) printf("%02X", public_key[i]);
    printf("\n");

    return 0;
}
