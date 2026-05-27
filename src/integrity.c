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
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
#endif

static const unsigned char PUBLIC_KEY[32] = {
     0xB1, 0x78, 0x32, 0x30, 0x71, 0xA4, 0xFF, 0x79,
    0xA5, 0xBC, 0x69, 0x08, 0x48, 0x7A, 0xA3, 0x59,
    0x30, 0xD8, 0x7B, 0xCD, 0x17, 0x6A, 0x7A, 0x42,
    0x83, 0x2F, 0xB1, 0xE0, 0x89, 0x66, 0x7D, 0x8C
};

/**
 * Checa a integridade do proprio arquivo executavel anexado da assinatura final
 * Retorna: 0 (Sucesso), 1 (Falha de Assinatura), 2 (Erro do Sistema/Arquivo)
 */
int check_integrity(void) {
    char path[1024];
    
    #ifdef _WIN32
        if (GetModuleFileNameA(NULL, path, sizeof(path)) == 0) return 2;
    #elif defined(__APPLE__)
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0) return 2;
    #else
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len <= 0) {
            return 2;
        }
        path[len] = '\0';
    #endif

    FILE *f = fopen(path, "rb");
    if (!f) return 2;

    fseek(f, 0, SEEK_END);
    long total_size = ftell(f);
    if (total_size <= 128) { 
        fclose(f); 
        return 1;
    }

    fseek(f, total_size - 128, SEEK_SET);
    char sig_hex[129] = {0};
    if (fread(sig_hex, 1, 128, f) != 128) { 
        fclose(f); 
        return 2;
    }

    unsigned char signature[64];
    for (int i = 0; i < 64; i++) {
        unsigned int byte;
        if (sscanf(sig_hex + i*2, "%2x", &byte) != 1) { 
            fclose(f); 
            return 1;
        }
        signature[i] = (unsigned char)byte;
    }

    size_t data_len = total_size - 128;
    unsigned char *data = malloc(data_len);
    if (!data) { 
        fclose(f); 
        return 2;
    }
    
    fseek(f, 0, SEEK_SET);
    size_t bytes_read = fread(data, 1, data_len, f);
    fclose(f);
    
    if (bytes_read != data_len) {
        free(data);
        return 2;
    }

    int result = crypto_eddsa_check(signature, PUBLIC_KEY, data, data_len);
    free(data);
    return (result == 0) ? 0 : 1; 
}