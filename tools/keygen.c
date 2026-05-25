#include <stdio.h>
#include <stdlib.h>
#include "../src/monocypher.h"
#include "../src/monocypher.c"

int main() {
    uint8_t seed[32];
    uint8_t secret_key[64];
    uint8_t public_key[32];
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) {
        perror("Erro ao abrir /dev/urandom");
        return 1;
    }
    fread(seed, 1, 32, f);
    fclose(f);
    memcpy(secret_key, seed, 32);
    crypto_eddsa_key_pair(secret_key, public_key, secret_key);
    f = fopen("../keys/NeonX.key", "wb");
    if (f) {
        fwrite(secret_key, 1, 64, f);
        fclose(f);
        printf("Chave privada salva em ../keys/NeonX.key\n");
    }

    f = fopen("../keys/NeonX.pub", "w");
    if (f) {
        for (int i = 0; i < 32; i++) {
            fprintf(f, "%02X", public_key[i]);
        }
        fprintf(f, "\n");
        fclose(f);
        printf("Chave pública salva em ../keys/NeonX.pub\n");
    }

    printf("\nChave pública formatada:\n\n");
    for (int i = 0; i < 32; i++) {
        printf("0x%02X", public_key[i]);        
        if (i < 31) {
            printf(", ");
        }
        if ((i + 1) % 8 == 0) {
            printf("\n    ");
        }
    }
    printf("\n");
}
