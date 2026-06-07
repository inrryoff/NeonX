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
#include "monocypher.h"
#include "../src/core/monocypher.c"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_binario> <chave_privada.key>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen binario (leitura)"); return 1; }
    
    crypto_blake2b_ctx blake_ctx;
    crypto_blake2b_init(&blake_ctx, 64);
    
    unsigned char buffer[8192];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        crypto_blake2b_update(&blake_ctx, buffer, read_bytes);
    }
    fclose(f);
    
    unsigned char hash[64];
    crypto_blake2b_final(&blake_ctx, hash);

    f = fopen(argv[2], "rb");
    if (!f) { perror("fopen chave"); return 1; }
    unsigned char secret_key[64];
    if (fread(secret_key, 1, 64, f) != 64) {
        fprintf(stderr, "Erro ao ler chave privada\n");
        fclose(f);
        return 1;
    }
    fclose(f);

    unsigned char signature[64];
    crypto_eddsa_sign(signature, secret_key, hash, 64);

    // Retornamos a assinatura via STDOUT para compatibilidade com o builder original
    for (int i = 0; i < 64; i++) {
        printf("%02X", signature[i]);
    }
    
    return 0;
}
