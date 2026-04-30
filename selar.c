#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIG_SIZE 8

void simple_cipher(const unsigned char *key, int key_len, 
                   const unsigned char *msg, int msg_len, 
                   unsigned char *output) {
    for (int i = 0; i < msg_len; ++i) {
        output[i] = msg[i] ^ key[i % key_len];
    }
}

void bytes_to_hex(const unsigned char *bytes, int len, char *hex_out) {
    for (int i = 0; i < len; ++i) {
        snprintf(hex_out + (i * 2), 3, "%02x", bytes[i]);
    }
    hex_out[len * 2] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <binario> <chave_secreta>\n", argv[0]);
        return 1;
    }
    
    char *binary_path = argv[1];
    char *secret = argv[2];
    int secret_len = strlen(secret);
    
    FILE *f = fopen(binary_path, "rb");
    if (!f) {
        fprintf(stderr, "Erro ao abrir: %s\n", binary_path);
        return 1;
    }
    
    unsigned int hash = 2166136261u;
    int c;
    while ((c = fgetc(f)) != EOF) {
        hash ^= (unsigned char)c;
        hash *= 16777619u;
    }
    fclose(f);
    
    printf("%08X\n", hash);
    
    char hash_str[9];
    snprintf(hash_str, sizeof(hash_str), "%08X", hash);

    unsigned char challenge[256];
    simple_cipher((unsigned char *)hash_str, 8,
                  (unsigned char *)secret, secret_len,
                  challenge);

    char hex_output[513];
    bytes_to_hex(challenge, secret_len, hex_output);

    printf("%s", hex_output);
    
    return 0;
}

