#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/monocypher.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_binario> <chave_privada.key>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen binario"); return 1; }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *data = malloc(size);
    if (!data) { fclose(f); return 1; }
    fread(data, 1, size, f);
    fclose(f);

    f = fopen(argv[2], "rb");
    if (!f) { perror("fopen chave"); free(data); return 1; }
    unsigned char secret_key[64];
    fread(secret_key, 1, 64, f);
    fclose(f);

    unsigned char signature[64];
    crypto_eddsa_sign(signature, secret_key, data, size);

    for (int i = 0; i < 64; i++) printf("%02X", signature[i]);
    free(data);
    return 0;
}