/*
 * NeonX — Terminal Shader Engine
 * Copyright (C) 2026  inrryoff
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "monocypher.h"
#include "../src/core/monocypher.c"

#ifdef _WIN32
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <chave_privada.key> <chave_publica.pub> [--print-hex]\n", argv[0]);
        return 1;
    }

    uint8_t seed[32];
    uint8_t secret_key[64];
    uint8_t public_key[32];

    FILE *f = fopen("/dev/urandom", "rb");
    if (f) {
        if (fread(seed, 1, 32, f) != 32) {
            // Fallback se falhar a leitura
            for (int i = 0; i < 32; i++) seed[i] = (uint8_t)(rand() % 256);
        }
        fclose(f);
    } else {
        // Fallback para Windows ou sistemas sem /dev/urandom
        srand((unsigned int)(time(NULL) ^ (unsigned int)getpid()));
        for (int i = 0; i < 32; i++) {
            seed[i] = (uint8_t)(rand() % 256);
        }
    }

    memcpy(secret_key, seed, 32);
    crypto_eddsa_key_pair(secret_key, public_key, secret_key);

    f = fopen(argv[1], "wb");
    if (!f) { perror("Erro ao abrir arquivo de chave privada"); return 1; }
    fwrite(secret_key, 1, 64, f);
    fclose(f);

    f = fopen(argv[2], "wb");
    if (!f) { perror("Erro ao abrir arquivo de chave pública"); return 1; }
    fwrite(public_key, 1, 32, f);
    fclose(f);

    // Se solicitado, imprime a hex da pública no formato C para o src/integrity.h
    if (argc > 3 && strcmp(argv[3], "--print-hex") == 0) {
        printf("static const unsigned char NEONX_OFFICIAL_PUBLIC_KEY[32] = {\n");
        for (int i = 0; i < 32; i++) {
            printf("0x%02X%s", public_key[i], (i == 31) ? "" : ", ");
            if ((i + 1) % 8 == 0 && i != 31) printf("\n    ");
        }
        printf("\n};\n");
    }

    return 0;
}
