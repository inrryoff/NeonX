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
    0xF1, 0x66, 0x0F, 0x10, 0x9F, 0xA2, 0xE2, 0x4E,
    0x0C, 0x00, 0xE8, 0xB8, 0xC5, 0xDA, 0xD3, 0x5C,
    0x7F, 0xEB, 0x11, 0x0D, 0x71, 0x39, 0x5C, 0x1C,
    0x7F, 0xB4, 0x67, 0x5D, 0x1C, 0x06, 0x27, 0x64
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
    size_t bytes_read = fread(data, 1, data_len, f);
    fclose(f);
    if (bytes_read != data_len) {
        free(data);
        return 2;
    }

    int result = crypto_eddsa_check(signature, PUBLIC_KEY, data, data_len);
    free(data);
    return (result == 0) ? 0 : 2;
}
