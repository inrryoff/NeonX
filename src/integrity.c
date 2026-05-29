#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "msgs.h"
#include "integrity.h"
#include "monocypher.h" // Biblioteca de criptografia externa

// Configuração de diretórios específicos de acordo com o sistema operacional
#ifdef __linux__
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

// Chave pública (32 bytes) gravada diretamente no código-fonte (hardcoded).
// É usada para validar a assinatura anexada no final do binário executável.

static unsigned char active_public_key[32];
static bool key_loaded = false;
static bool using_official = false;

static int hex2bin(uint8_t *bin, const uint8_t *hex, size_t hex_len) {
    if (hex_len % 2 != 0) return -1;
    for (size_t i = 0; i < hex_len / 2; i++) {
        uint8_t h_char = hex[i * 2];
        uint8_t l_char = hex[i * 2 + 1];
        int h, l;
        if (h_char >= '0' && h_char <= '9') h = h_char - '0';
        else if (h_char >= 'A' && h_char <= 'F') h = h_char - 'A' + 10;
        else if (h_char >= 'a' && h_char <= 'f') h = h_char - 'a' + 10;
        else return -1;
        if (l_char >= '0' && l_char <= '9') l = l_char - '0';
        else if (l_char >= 'A' && l_char <= 'F') l = l_char - 'A' + 10;
        else if (l_char >= 'a' && l_char <= 'f') l = l_char - 'a' + 10;
        else return -1;
        bin[i] = (uint8_t)((h << 4) | l);
    }
    return 0;
}

static void load_active_key(void)
{
    if (key_loaded) return;

    const char *env_path = getenv("NEONX_KEY_FILE");
    if (env_path) {
        FILE *f = fopen(env_path, "rb");
        if (f) {
            if (fread(active_public_key, 1, 32, f) == 32) {
                fclose(f);
                key_loaded = true;
                using_official = false; // Chave externa é considerada não oficial/desenvolvedor
                return;
            }
            fclose(f);
            fprintf(stderr, "%s", MSG(MSG_WARN_KEY_LOAD_FAIL));
        }
    }

#ifdef GENERIC_PUBLIC_KEY
    if (hex2bin(active_public_key, (const uint8_t*)GENERIC_PUBLIC_KEY, 64) == 0) {
        key_loaded = true;
        using_official = false;
        return;
    }
#endif

    // Default: usa a chave embutida no binário
    memcpy(active_public_key, NEONX_OFFICIAL_PUBLIC_KEY, 32);
    key_loaded = true;
    using_official = true;
}

bool is_using_official_key(void) {
    load_active_key();
    return using_official;
}

int check_integrity(void) {
    load_active_key();
    char exec_path[1024] = {0};
#ifdef __linux__
    ssize_t len = readlink("/proc/self/exe", exec_path, sizeof(exec_path)-1);
    if (len == -1) return 2;
#elif defined(_WIN32)
    GetModuleFileNameA(NULL, exec_path, sizeof(exec_path));
#else
    return 2;
#endif

    FILE *f = fopen(exec_path, "rb");
    if (!f) return 2;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    if (file_size < 128) {
        fclose(f);
        return 1;
    }

    uint8_t hex_signature[128];
    fseek(f, -128, SEEK_END);
    if (fread(hex_signature, 1, 128, f) != 128) {
        fclose(f);
        return 2;
    }

    uint8_t bin_signature[64];
    if (hex2bin(bin_signature, hex_signature, 128) != 0) {
        fclose(f);
        return 1;
    }

    fseek(f, 0, SEEK_SET);
    crypto_blake2b_ctx blake_ctx;
    crypto_blake2b_init(&blake_ctx, 64);
    
    uint8_t buffer[8192];
    long remaining = file_size - 128;
    
    while (remaining > 0) {
        size_t to_read = (remaining > (long)sizeof(buffer)) ? sizeof(buffer) : (size_t)remaining;
        size_t read_bytes = fread(buffer, 1, to_read, f);
        if (read_bytes == 0) break;
        crypto_blake2b_update(&blake_ctx, buffer, read_bytes);
        remaining -= (long)read_bytes;
    }
    fclose(f);
    
    uint8_t hash[64];
    crypto_blake2b_final(&blake_ctx, hash);

    if (crypto_eddsa_check(bin_signature, active_public_key, hash, 64) == 0) {
        return 0;
    }
    
    return 1;
}
