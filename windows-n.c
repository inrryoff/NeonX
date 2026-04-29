#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <wchar.h>
#include <windows.h>
#include <io.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define STDOUT_FILENO 1
#define write _write
#define wcsdup _wcsdup

const char* ORIGINAL_CREATOR = "inrryoff";
#ifndef BUILD_MAINTAINER
    #define BUILD_MAINTAINER "Comunidade_(Nao Oficial)"
#endif
#ifndef VERSION
    #define VERSION "1.6.1-TERMUX"
#endif
#ifndef BUILD_STATUS
    #define BUILD_STATUS "GENERIC_BUILD_BY_COMMUNITY"
#endif
#ifndef GITHUB_REPO
    #define GITHUB_REPO "https://github.com/inrryoff/NeonX"
#endif
#ifndef CHAVE_SECRETA_NEONX
    #define CHAVE_SECRETA_NEONX "BUILD_GENERICA_INFERIOR_26"
#endif
#define SIG_SIZE 26
#define MAX_LINES 1024
#define MAX_LINE_LEN 2048
#define LUT_SIZE 4096
#define PI_2 1.57079632679

typedef struct { 
    int r, g, b; 
} RGBColor;

typedef struct { 
    wchar_t *lines[MAX_LINES]; 
    int count; 
} Content;

int fixed_width = 0;
double freq = 0.3, diagonal = 0.0, opacity = 1.0;
double frame_time_us = 50000;
double sin_lut[LUT_SIZE];
bool is_truecolor = false;
Content content = {.count = 0};

bool check_auth(void);
void free_content(Content *c);
void enable_ansi_windows(void);

bool check_auth(void) { 
    return (strcmp(ORIGINAL_CREATOR, "inrryoff") == 0); 
}

double shader_sunset(int x, int y, double phase) {
    return sin(x * 0.15 + phase) + sin(y * 0.15 + phase * 0.5);
}

double shader_matrix(int x, int y, double phase, double *intensity) {
    double p = phase + x * 0.1 + y * 0.1;
    double pulse = sin(phase * 3.0) * 0.15 + 0.85;
    double scanline = (fmod(y - phase * 5.0, 10.0) < 1.0) ? 0.7 : 1.0;
    *intensity = pulse * scanline * ((rand() % 100 > 98) ? 0.5 : 1.0);
    return p;
}

double shader_pulse(int x, int y, int len, int count, double phase, double *intensity) {
    double dx = x - len / 2.0;
    double dy = y - count / 2.0;
    double dist = sqrt(dx * dx + dy * dy);
    double p = dist - phase;
    *intensity = (sin(dist * 0.5 - phase * 2.0) + 1.0) / 2.0;
    *intensity = *intensity * 0.8 + 0.2;
    return p;
}

void sleep_us(double microseconds) {
    Sleep((DWORD)(microseconds / 1000.0));
}

void enable_ansi_windows(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void free_content(Content *c) {
    for(int i=0; i<c->count; i++) if(c->lines[i]) free(c->lines[i]);
    c->count = 0;
}

bool verify_self_integrity(void) {
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return false;
    
    FILE *f = fopen(path, "rb");
    if (!f) return false;
    
    fseek(f, -SIG_SIZE, SEEK_END);
    char signature[SIG_SIZE + 1] = {0};
    fread(signature, 1, SIG_SIZE, f);
    fclose(f);
    for (int i = 0; i < SIG_SIZE; i++) {
        if (signature[i] == ' ') { return false; }
    }
    
    return (memcmp(signature, CHAVE_SECRETA_NEONX, SIG_SIZE) == 0);
}

void security_trigger(void) {
    fprintf(stderr, "\n\033[1;31m[FATAL] Integridade do NeonX violada.\n\033[0m");
    free_content(&content);
    exit(1);
}

void print_version(void) {
    printf("NeonX v%s\n", VERSION);
    printf("Criador Original: %s\n", ORIGINAL_CREATOR);
    printf("Compilado por: %s\n", BUILD_MAINTAINER);
    printf("Status: %s\n", BUILD_STATUS);
}

void print_license(void) {
    wprintf(L"LICENÇA DE USO - NEONX (C - VERSION)\n");
    wprintf(L"-----------------------------------------------------------------\n");
    wprintf(L"Copyright (c) 2024 @inrryoff - Licenciado sob condições especiais NeonX LICENSE\n\n");
    wprintf(L"Pelo presente, fica concedida permissão a qualquer pessoa que obtenha uma cópia\n");
    wprintf(L"deste software para usá-lo gratuitamente, sujeito às seguintes condições:\n\n");  
    wprintf(L"1. ATRIBUIÇÃO (CRÉDITOS):\n");
    wprintf(L"   O nome do autor original (@inrryoff) e os avisos de copyright devem ser\n");
    wprintf(L"   mantidos em todos os arquivos de código-fonte, cabeçalhos e na saída de\n");
    wprintf(L"   versão do binário compilado (ex: neonx --version).\n\n");  
    wprintf(L"2. PROIBIÇÃO DE COMERCIALIZAÇÃO:\n");
    wprintf(L"   É TERMINANTEMENTE PROIBIDA a venda, aluguel ou qualquer forma de\n");
    wprintf(L"   comercialização deste software, seja do código-fonte ou do binário\n");
    wprintf(L"   compilado, de forma isolada ou integrada a pacotes pagos.\n\n");  
    wprintf(L"3. DERIVAÇÕES E MODIFICAÇÕES:\n");
    wprintf(L"   Alterações no código são permitidas para melhorias ou uso pessoal, desde que:\n");
    wprintf(L"   a) O trabalho derivado NÃO seja vendido.\n");
    wprintf(L"   b) A versão modificada seja mantida em repositório público (Open Source).\n");
    wprintf(L"   c) Os créditos ao autor original sejam mantidos de forma clara.\n\n");  
    wprintf(L"4. DISTRIBUIÇÃO EM MÓDULOS (MAGISK/KERNELSU):\n");
    wprintf(L"   O uso deste binário em módulos de otimização é permitido e encorajado,\n");
    wprintf(L"   desde que o módulo seja distribuído gratuitamente.\n\n");
    wprintf(L"O SOFTWARE É FORNECIDO 'COMO ESTÁ', SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU\n");
    wprintf(L"IMPLÍCITA. EM NENHUM EVENTO O AUTOR SERÁ RESPONSÁVEL POR QUALQUER RECLAMAÇÃO,\n");  
    wprintf(L"DANOS OU OUTRA RESPONSABILIDADE RESULTANTE DO USO DESTE SOFTWARE.\n");
}

void show_help(void) {
    wprintf(L"NeonX v%s | Core por: %s | Build por: %s\n\n", VERSION, ORIGINAL_CREATOR, BUILD_MAINTAINER);
    wprintf(L"Uso: cat arquivo | neonx [opcoes]\n\n");
    wprintf(L"-m [0-11]      Modos de animação\n");
    wprintf(L"-s [valor]     Velocidade (0.2 padrao)\n");
    wprintf(L"-f [valor]     Frequência (0.3 padrao)\n");
    wprintf(L"-d [valor]     Duração (0: infinito)\n");
    wprintf(L"-D [valor]     Inclinação diagonal\n");
    wprintf(L"-p [valor]     Seeds fixas\n");
    wprintf(L"-S             Modo estático\n");
    wprintf(L"-c [largura]   Largura fixa do gradiente\n");
    wprintf(L"-o [0-1]       Opacidade das bordas\n");
    wprintf(L"-F [valor]     FPS (ex: 60, 90)\n");
    wprintf(L"-L             Modo linha por linha (stream)\n");
    wprintf(L"--preset [nome] Carregar preset (cyberpunk, retro, matrix, sunset)\n");
    wprintf(L"--termux       Modo de compatibilidade com Termux\n");
    wprintf(L"--spin         Lista de cores para spinner\n");
    wprintf(L"--license      Licença de software\n");
    wprintf(L"-v,--version   Versão do binário\n");
    wprintf(L"-h,--help      Exibe esta ajuda\n");
}

void init_lut() { 
    for(int i=0; i<LUT_SIZE; i++) sin_lut[i] = sin(2.0*M_PI*i/LUT_SIZE);
}

static inline double fast_sin(double x) {
    int idx = (int)(fmod(x*LUT_SIZE/(2.0*M_PI), LUT_SIZE));
    if(idx < 0) idx += LUT_SIZE;
    return sin_lut[idx];
}

void apply_border_opacity(int x, int y, int len, int count, double op, int *r, int *g, int *b) {
    if (op <= 0.0) return;
    double cx = len / 2.0, cy = count / 2.0;
    double dx = x - cx, dy = y - cy;
    double factor = 1.0 - (sqrt(dx*dx + dy*dy) / sqrt(cx*cx + cy*cy));
    factor = 1.0 - (1.0 - factor) * op;
    *r = (int)(*r * factor);
    *g = (int)(*g * factor);
    *b = (int)(*b * factor);
    if(*r<0) *r=0; else if(*r>255) *r=255;
    if(*g<0) *g=0; else if(*g>255) *g=255;
    if(*b<0) *b=0; else if(*b>255) *b=255;
}

void get_color_fast(int x, int y, int mode, int len, int count, double phase, int *r, int *g, int *b) {
    double p, intensity = 1.0;
    switch(mode) {  
        case 1: p = shader_sunset(x, y, phase); break;
        case 2: p = phase; break;
        case 3: p = y*0.5+phase; break;
        case 4: p = y*0.8+fast_sin(x*0.3)*3.0+phase; break;
        case 5: { double dx=x-len/2.0, dy=y-count/2.0; p=sqrt(dx*dx+dy*dy)-phase; break; }  
        case 6: p = fast_sin(x*0.2)*fast_sin(y*0.2+PI_2)*10.0+phase; break;  
        case 7: { double center=len/2.0; p=phase+x*0.1; intensity=1.0-(fabs(x-center)/center)*0.8; if(intensity<0.2) intensity=0.2; break;}  
        case 8: { double center_y=count/2.0; p=phase+y*0.2; intensity=1.0-(fabs(y-center_y)/center_y)*0.7; if(intensity<0.3) intensity=0.3; break;}  
        case 9: p = phase+x*0.1; intensity=(x%2==0)?1.0:0.2; break;
        case 10: p = shader_matrix(x, y, phase, &intensity); break;  
        case 11: p = shader_pulse(x, y, len, count, phase, &intensity); break;
        default: p = phase+x*0.2+y*0.1+(diagonal*(x+y)); break;  
    }
    
    *r = (int)((fast_sin(freq*p+0)*127+128)*intensity);
    *g = (int)((fast_sin(freq*p+2.094)*127+128)*intensity);
    *b = (int)((fast_sin(freq*p+4.188)*127+128)*intensity);
    
    apply_border_opacity(x, y, len, count, opacity, r, g, b);
}

void handle_sigint(int sig) {
    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 17);
    free_content(&content);
    exit(0);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    enable_ansi_windows();
    init_lut();
    srand((unsigned int)time(NULL)); 
    signal(SIGINT, handle_sigint);
    
    if (!check_auth() || !verify_self_integrity()) security_trigger();
    
    bool static_mode = false, stream_mode = false;
    int anim_mode = 0;
    double speed = 0.2, duration = 0;
    double start_phase = -1.0;
    
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (!strcmp(arg,"-h") || !strcmp(arg,"--help")) { show_help(); return 0; }
        if (!strcmp(arg,"-v") || !strcmp(arg,"--version")) { print_version(); return 0; }
        if (!strcmp(arg,"--license")) { print_license(); return 0; }
        if (!strcmp(arg,"-S")) { static_mode = true; continue; }
        if (!strcmp(arg,"-L")) { stream_mode = true; continue; }

        if (!strcmp(arg,"--spin")) {
            for (int j = 0; j < 60; j++) {
                int r=(int)(fast_sin(freq*j*0.4+0)*127+128);
                int g=(int)(fast_sin(freq*j*0.4+2.094)*127+128);
                int b=(int)(fast_sin(freq*j*0.4+4.188)*127+128);
                printf("38;2;%d;%d;%d ", r,g,b);
            } printf("\n"); return 0;
        }

        if (!strcmp(arg,"--preset") && i+1 < argc) {
            i++;
            if (!strcmp(argv[i],"cyberpunk")) { anim_mode=0; speed=0.3; freq=0.5; diagonal=0.1; }
            else if (!strcmp(argv[i],"retro")) { anim_mode=4; speed=0.2; freq=0.8; diagonal=0.0; }
            else if (!strcmp(argv[i],"matrix")) { anim_mode=10; speed=0.5; freq=1.2; diagonal=0.0; }
            else if (!strcmp(argv[i],"sunset")) { anim_mode=1; speed=0.15; freq=0.3; diagonal=0.05; }
            continue;
        }

        if ((i+1)<argc) {
            char *val = argv[i+1];
            if (!strcmp(arg,"-d")) { duration = atof(val); i++; continue; }
            if (!strcmp(arg,"-s")) { speed = atof(val); i++; continue; }
            if (!strcmp(arg,"-f")) { freq = atof(val); i++; continue; }
            if (!strcmp(arg,"-m")) { int tmp=atoi(val); if(tmp>=0&&tmp<=11) anim_mode=tmp; i++; continue; }
            if (!strcmp(arg,"-D")) { diagonal=atof(val); i++; continue; }
            if (!strcmp(arg,"-c")) { fixed_width=atoi(val); i++; continue; }
            if (!strcmp(arg,"-o")) { opacity=atof(val); i++; continue; }
            if (!strcmp(arg,"-p")) { start_phase = atof(val); i++; continue; }
            if (!strcmp(arg,"-F")) { frame_time_us=1000000.0/atof(val); i++; continue; }
        }
    }

    if (stream_mode) {
        wchar_t buffer[MAX_LINE_LEN];
        double phase = (start_phase >= 0.0) ? start_phase : (rand() % 1000) / 10.0;
        int line_count = 0;
        write(STDOUT_FILENO, "\033[?7l\033[?25l", 12);
        while (fgetws(buffer, MAX_LINE_LEN, stdin)) {
            size_t len = wcslen(buffer);
            if (len > 0 && buffer[len-1] == L'\n') buffer[len-1] = L'\0';
            
            for (size_t k = 0; k < len; k++) { if (buffer[k] == 0x1B) buffer[k] = L'?'; }
            
            for (size_t x = 0; x < len; x++) {
                int r,g,b;
                get_color_fast((int)x, line_count, anim_mode, (int)len, 1, phase, &r, &g, &b);
                printf("\033[38;2;%d;%d;%dm%lc", r, g, b, buffer[x]);
            }
            printf("\033[0m\n");
            fflush(stdout);
            phase += speed;
            line_count++;
        }
        write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 17);
        return 0;
    }

    wchar_t buf[MAX_LINE_LEN];
    while (fgetws(buf, MAX_LINE_LEN, stdin) && content.count < MAX_LINES) {
        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        
        for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; }
        
        content.lines[content.count++] = wcsdup(buf);
    }
    if (content.count == 0) return 0;

    int max_w = 0;
    for(int i=0; i<content.count; i++) { 
        int l = wcslen(content.lines[i]); 
        if(l>max_w) max_w=l;
    }
    if (fixed_width > 0) max_w = fixed_width;
    
    size_t buf_size = content.count * (max_w * 32 + 100);
    char *frame_buf = malloc(buf_size);
    RGBColor *color_cache = malloc(max_w * sizeof(RGBColor));
    if (!frame_buf || !color_cache) {
        free_content(&content);
        if (frame_buf) free(frame_buf);
        if (color_cache) free(color_cache);
        return 1;
    }

    #define SAFE_APPEND(...) do { \
        size_t rem = buf_size - (ptr - frame_buf); \
        int n = snprintf(ptr, rem, __VA_ARGS__); \
        if (n > 0 && (size_t)n < rem) ptr += n; \
    } while(0)

    double phase = (start_phase >= 0.0) ? start_phase : (rand() % 1000) / 10.0;
    time_t start_time = time(NULL);
    write(STDOUT_FILENO, "\033[?25l\033[?7l", 12);
    bool first_frame = true;
    
    while (1) {
        if (duration > 0 && difftime(time(NULL), start_time) > duration) break;
        
        char *ptr = frame_buf;
        if (!first_frame) { SAFE_APPEND("\033[%dA", content.count); }
        first_frame = false;
        
        for (int y = 0; y < content.count; y++) {
            wchar_t *line = content.lines[y];
            int line_len = wcslen(line);
            
            for (int x = 0; x < max_w; x++) {
                get_color_fast(x, y, anim_mode, max_w, content.count, phase,
                               &color_cache[x].r, &color_cache[x].g, &color_cache[x].b);
            }
            
            SAFE_APPEND("\r");
            for (int x = 0; x < line_len; x++) {
                SAFE_APPEND("\033[38;2;%d;%d;%dm%lc", color_cache[x].r, color_cache[x].g, color_cache[x].b, line[x]);
            }
            SAFE_APPEND("\033[0m\033[K\n");
        }
        write(STDOUT_FILENO, frame_buf, ptr - frame_buf);
        
        if (static_mode) break;
        phase += speed;
        sleep_us(frame_time_us);
    }

    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m", 15);
    free_content(&content);
    free(frame_buf);
    free(color_cache);
    return 0;
}

