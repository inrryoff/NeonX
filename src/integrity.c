#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "integrity.h"
#include "monocypher.h"

#ifdef _WIN32
    #include <windows.h>
    #ifndef MAX_PATH
        #define MAX_PATH 260
    #endif
#else
    #include <unistd.h>
#endif

static const unsigned char PUBLIC_KEY[32] = {
    0xF6, 0xB5, 0x44, 0x07, 0x02, 0x79, 0x48, 0xF3,
    0xC5, 0x8D, 0x5D, 0x04, 0x52, 0xAD, 0x6D, 0xBE,
    0xE1, 0x3A, 0x17, 0x61, 0x9B, 0xAF, 0x1E, 0x84,
    0x87, 0x3A, 0x7E, 0x2C, 0x97, 0x66, 0x3B, 0xB8
};

int check_integrity(void) {
    char path[1024];
    #ifdef _WIN32
        if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return 2;
    #else
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len == -1) return 2;
        path[len] = '\0';
    #endif

    FILE *f = fopen(path, "rb");
    if (!f) return 2;

    fseek(f, 0, SEEK_END);
    long total_size = ftell(f);
    if (total_size <= 128) { fclose(f); return 2; }

    fseek(f, total_size - 128, SEEK_SET);
    char sig_hex[129] = {0};
    if (fread(sig_hex, 1, 128, f) != 128) { fclose(f); return 2; }

    unsigned char signature[64];
    for (int i = 0; i < 64; i++) {
        unsigned int byte;
        if (sscanf(sig_hex + i*2, "%2x", &byte) != 1) { fclose(f); return 2; }
        signature[i] = (unsigned char)byte;
    }

    size_t data_len = total_size - 128;
    unsigned char *data = malloc(data_len);
    if (!data) { fclose(f); return 2; }
    fseek(f, 0, SEEK_SET);
    fread(data, 1, data_len, f);
    fclose(f);

    int result = crypto_eddsa_check(signature, PUBLIC_KEY, data, data_len);
    free(data);
    return (result == 0) ? 0 : 2;
}
