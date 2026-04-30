#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    FILE *f = fopen(argv[1], "rb");
    if (!f) return 1;

    unsigned int hash = 2166136261u;
    int c;
    while ((c = fgetc(f)) != EOF) {
        hash ^= (unsigned char)c;
        hash *= 16777619u;
    }
    fclose(f);

    printf("%08X\n", hash);
    return 0;
}