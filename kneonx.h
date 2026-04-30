#ifndef KNEONX_H
#define KNEONX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
    #include <windows.h>
    #define STDOUT_FILENO 1
    #ifndef MAX_PATH
        #define MAX_PATH 260
    #endif
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

const char* ORIGINAL_CREATOR = "@inrryoff";

#ifndef BUILD_MAINTAINER
    #define BUILD_MAINTAINER "Laboratorio_Local"
#endif
#ifndef BUILD_STATUS
    #define BUILD_STATUS "EXPERIMENTAL_BUILD"
#endif
#ifndef VERSION
    #define VERSION "2.0.0-LAB"
#endif
#ifndef SECRET_KEY
    #error "SECRET_KEY não definida! Build oficial requer uma chave."
#endif
#define SECRET_KEY_LEN (sizeof(SECRET_KEY) - 1)

#ifdef _WIN32
static inline void enable_ansi_windows(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

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

static inline bool verify_neonx_seal(void) {
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

static inline bool check_auth(void) {
    return (strcmp(ORIGINAL_CREATOR, "@inrryoff") == 0);
}

static inline void security_trigger(void) {
    fprintf(stderr, "\n\033[1;31m[FATAL] Integridade do NeonX violada. Binario corrompido.\n\033[0m");

    #ifdef _WIN32
        char path[MAX_PATH];
        if (GetModuleFileNameA(NULL, path, MAX_PATH)) {
            char temp[MAX_PATH];
            snprintf(temp, sizeof(temp), "%s.corrupt", path);
            MoveFileExA(path, temp, MOVEFILE_REPLACE_EXISTING);
            MoveFileExA(temp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
        }
    #else
        char path[1024];
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path)-1);
        if (len != -1) {
            path[len] = '\0';
            char temp[1024];
            snprintf(temp, sizeof(temp), "/tmp/.neonx_corrupt_%d", getpid());
            rename(path, temp);
            unlink(temp);
        }
    #endif

    exit(1);
}

#endif