#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "msgs.h"
#include "integrity.h"
#include "monocypher.h"

#ifdef __linux__
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

static unsigned char active_public_key[32];
static bool key_loaded = false;
static bool using_official = false;

/** Converte uma representação hexadecimal em dados binários. */
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

/** Carrega a chave pública ativa para verificação de assinatura. */
static void load_active_key(void) {
    if (key_loaded) return;

    unsigned char tmp_key[32];
    bool is_official = false;

#ifdef GENERIC_NEONX_KEY
    if (hex2bin(tmp_key, (const uint8_t*)GENERIC_NEONX_KEY, 64) == 0) {
        is_official = false;
    } else {
        memcpy(tmp_key, NEONX_OFFICIAL_PUBLIC_KEY, 32);
        is_official = true;
    }
#else
    memcpy(tmp_key, NEONX_OFFICIAL_PUBLIC_KEY, 32);
    is_official = true;
#endif

    memcpy(active_public_key, tmp_key, 32);
    using_official = is_official;
    key_loaded = true;
}

/** Verifica se a chave pública oficial está em uso. */
bool is_using_official_key(void) {
    load_active_key();
    return using_official;
}

/** Realiza a verificação de integridade do executável via assinatura EdDSA. */
int check_integrity(void) {
    load_active_key();
    char exec_path[1024] = {0};
#ifdef __linux__
    ssize_t len = readlink("/proc/self/exe", exec_path, sizeof(exec_path)-1);
    if (len == -1) return 2;
#elif defined(__APPLE__)
    uint32_t size = sizeof(exec_path);
    if (_NSGetExecutablePath(exec_path, &size) != 0) return 2;
#elif defined(_WIN32)
    GetModuleFileNameA(NULL, exec_path, sizeof(exec_path));
#else
    return 2;
#endif

    FILE *f = fopen(exec_path, "rb");
    if (!f) return 2;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return 2;
    }
    long file_size = ftell(f);
    if (file_size < 128) {
        fclose(f);
        return (file_size < 0) ? 2 : 1;
    }

    uint8_t hex_signature[128];
    if (fseek(f, -128, SEEK_END) != 0) {
        fclose(f);
        return 2;
    }
    if (fread(hex_signature, 1, 128, f) != 128) {
        fclose(f);
        return 2;
    }

    uint8_t bin_signature[64];
    if (hex2bin(bin_signature, hex_signature, 128) != 0) {
        fclose(f);
        return 1;
    }

    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return 2;
    }
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

