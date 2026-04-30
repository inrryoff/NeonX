#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include "integrity.h"

#ifdef _WIN32
    #include <windows.h>
    #ifndef MAX_PATH
        #define MAX_PATH 260
    #endif
#else
    #include <unistd.h>
#endif

#ifndef SECRET_KEY
    #define SECRET_KEY "CHAVE_TESTE_LOCAL"
#endif
#define SECRET_KEY_LEN (sizeof(SECRET_KEY) - 1)
#define CACHE_DIR_NAME  ".neonx_cache"
#define CACHE_FILE_NAME "allow_mod"

const char* ORIGINAL_CREATOR = "@inrryoff";

/* ============================================================
 * Cifra simples (XOR)
 * ============================================================ */
static inline void simple_cipher(const unsigned char *key, int key_len,
                                 const unsigned char *input, int input_len,
                                 unsigned char *output) {
    for (int i = 0; i < input_len; ++i) {
        output[i] = input[i] ^ key[i % key_len];
    }
}

static inline int hex_to_bytes(const char *hex, unsigned char *bytes) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    for (int i = 0; i < len; i += 2) {
        unsigned int byte;
        if (sscanf(hex + i, "%2x", &byte) != 1) return -1;
        bytes[i / 2] = (unsigned char)byte;
    }
    return len / 2;
}

/* ============================================================
 * Verificação do selo oficial (SECRET_KEY + FNV‑1a)
 * ============================================================ */
static bool verify_neonx_seal(void) {
    char path[1024];
    #ifdef _WIN32
        if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return false;
    #else
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len == -1) return false;
        path[len] = '\0';
    #endif

    FILE *f = fopen(path, "rb");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long total_size = ftell(f);
    long seal_hex_len = SECRET_KEY_LEN * 2;
    if (total_size < seal_hex_len) { fclose(f); return false; }

    fseek(f, -seal_hex_len, SEEK_END);
    char stored_seal_hex[513] = {0};
    if (fread(stored_seal_hex, 1, seal_hex_len, f) != (size_t)seal_hex_len) {
        fclose(f); return false;
    }
    stored_seal_hex[seal_hex_len] = '\0';

    unsigned char stored_challenge[256];
    int challenge_len = hex_to_bytes(stored_seal_hex, stored_challenge);
    if (challenge_len != SECRET_KEY_LEN) { fclose(f); return false; }

    fseek(f, 0, SEEK_SET);
    unsigned int hash = 2166136261u;
    long bytes_to_hash = total_size - seal_hex_len;
    for (long i = 0; i < bytes_to_hash; ++i) {
        int c = fgetc(f);
        if (c == EOF) break;
        hash ^= (unsigned char)c;
        hash *= 16777619u;
    }
    fclose(f);

    char current_hash_str[9];
    snprintf(current_hash_str, sizeof(current_hash_str), "%08X", hash);

    unsigned char decrypted_secret[256];
    simple_cipher((unsigned char *)current_hash_str, 8,
                  stored_challenge, challenge_len,
                  decrypted_secret);
    decrypted_secret[challenge_len] = '\0';

    return (memcmp(decrypted_secret, SECRET_KEY, SECRET_KEY_LEN) == 0);
}

/* ============================================================
 * check_integrity – apenas retorna status, sem imprimir nada
 * ============================================================ */
int check_integrity(void) {
    bool auth_ok = (strcmp(ORIGINAL_CREATOR, "@inrryoff") == 0);
    bool seal_ok = verify_neonx_seal();

    if (!auth_ok || !seal_ok) {
        return 2;   // não oficial
    }
    return 0;       // íntegro
}

/* ============================================================
 * Cache de permissão de modificação (allow_mod)
 * ============================================================ */

// Calcula o hash FNV-1a do binário inteiro (sem remover selo)
static bool get_binary_hash(unsigned char *hash_out) {
    char path[1024];
    #ifdef _WIN32
        if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return false;
    #else
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len == -1) return false;
        path[len] = '\0';
    #endif

    FILE *f = fopen(path, "rb");
    if (!f) return false;

    unsigned int hash = 2166136261u;
    int c;
    while ((c = fgetc(f)) != EOF) {
        hash ^= (unsigned char)c;
        hash *= 16777619u;
    }
    fclose(f);

    snprintf((char*)hash_out, 9, "%08X", hash);
    return true;
}

static bool get_cache_dir(char *dir, size_t size) {
    const char *home = getenv("HOME");
    if (!home) {
        #ifdef _WIN32
            home = getenv("USERPROFILE");
            if (!home) return false;
        #else
            return false;
        #endif
    }
    snprintf(dir, size, "%s/" CACHE_DIR_NAME, home);
    return true;
}

static bool get_cache_file_path(char *path, size_t size) {
    char dir[1024];
    if (!get_cache_dir(dir, sizeof(dir))) return false;
    snprintf(path, size, "%s/" CACHE_FILE_NAME, dir);
    return true;
}

static bool ensure_cache_dir(void) {
    char dir[1024];
    if (!get_cache_dir(dir, sizeof(dir))) return false;
    #ifdef _WIN32
        mkdir(dir);
    #else
        mkdir(dir, 0700);
    #endif
    return true;
}

int check_allow_mod_cache(void) {
    unsigned char current_hash[9];
    if (!get_binary_hash(current_hash)) return 0;

    char cache_path[1024];
    if (!get_cache_file_path(cache_path, sizeof(cache_path))) return 0;

    FILE *f = fopen(cache_path, "r");
    if (!f) return 0;

    char stored_hash[9] = {0};
    if (fread(stored_hash, 1, 8, f) != 8) {
        fclose(f);
        return 0;
    }
    fclose(f);
    stored_hash[8] = '\0';

    return (strncmp((const char*)current_hash, stored_hash, 8) == 0) ? 1 : 0;
}

void save_allow_mod_cache(void) {
    unsigned char current_hash[9];
    if (!get_binary_hash(current_hash)) return;

    if (!ensure_cache_dir()) return;

    char cache_path[1024];
    if (!get_cache_file_path(cache_path, sizeof(cache_path))) return;

    FILE *f = fopen(cache_path, "w");
    if (f) {
        fwrite(current_hash, 1, 8, f);
        fclose(f);
    }
}