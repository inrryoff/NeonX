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
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>

#include "integrity.h"
#include "shaders.h"
#include "terminal.h"
#include "msgs.h"

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #ifndef STDOUT_FILENO
        #define STDOUT_FILENO 1
    #endif
    #define write _write 
#else
    #include <unistd.h>
#endif

typedef struct { 
    int r, g, b; 
} RGBColor;

int fixed_width = 0;
uint32_t frame_time_us = 50000;

static uint64_t get_time_us(void) {
#ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    return (uint64_t)((now.QuadPart * 1000000LL) / freq.QuadPart);
#else
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
#endif
}

extern int idioma_atual;
extern Content content;
extern void free_content(Content *c);
extern void handle_sigint(int sig);
void init_lut(void);
void precalc_gradient_angle(void);
void msgs_init(void);
int check_integrity(void);
void set_integrity_status(int status);
void show_help(void);
void print_version(void);
void print_license(void);

static int32_t str_to_fixed(const char *s) {
    int32_t int_part = 0;
    int32_t frac_part = 0;
    int frac_len = 0;
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') {
        int_part = int_part * 10 + (*s - '0');
        s++;
    }
    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9' && frac_len < 6) {
            frac_part = frac_part * 10 + (*s - '0');
            frac_len++;
            s++;
        }
    }
    while (frac_len < 5) {
        frac_part *= 10;
        frac_len++;
    }
    int32_t frac_fixed = (int32_t)(((int64_t)frac_part * FIXED_ONE) / 100000);
    int32_t result = int_part * FIXED_ONE + frac_fixed;
    return sign * result;
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    init_lut();
    msgs_init();
    gradient_angle_fixed = FLOAT_TO_FIXED(-1.0f); 
    grad_cos_fixed = FIXED_ONE;
    grad_sin_fixed = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--lang") && i+1 < argc) {
            if (argv[i+1][0] != '-') {
                i++;
                if (strncmp(argv[i], "pt", 2) == 0) idioma_atual = 0;
                else if (strncmp(argv[i], "es", 2) == 0) idioma_atual = 2;
                else if (strncmp(argv[i], "zh", 2) == 0) idioma_atual = 3;
                else idioma_atual = 1;
            }
        }
    }
    
    signal(SIGINT, handle_sigint);
    srand((unsigned int)time(NULL));
    int auth_status = check_integrity();
    set_integrity_status(auth_status);
    
    bool static_mode = false, stream_mode = false;
    int anim_mode = 0;
    int32_t speed_fixed = FLOAT_TO_FIXED(0.2);
    int32_t duration_fixed = 0;
    uint64_t duration_us = 0;
    int32_t freq_fixed_local = FLOAT_TO_FIXED(0.3);
    int32_t start_phase_fixed = -FIXED_ONE;
    int32_t phase_fixed = 0;
    
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        
        if (!strcmp(arg,"-h") || !strcmp(arg,"--help")) { show_help(); return 0; }
        if (!strcmp(arg,"-v") || !strcmp(arg,"--version")) { print_version(); return 0; }
        if (!strcmp(arg,"--license")) { print_license(); return 0; }
        if (!strcmp(arg, "--lang")) { 
            if (i+1 < argc && argv[i+1][0] != '-') i++; 
            continue; 
        }
        if (!strcmp(arg, "--verify-sig")) {
            if (auth_status == 0) {
                printf("OK\n");
                return 0;
            } else {
                fprintf(stderr, "FAIL\n");
                return 1;
            }
        }
        if (!strcmp(arg,"-S")) { static_mode = true; continue; }
        if (!strcmp(arg,"-L")) { stream_mode = true; continue; }
        if (!strcmp(arg,"--quantized")) { use_quantization = true; continue; }
        if (!strcmp(arg, "--spin")) {
            const int32_t FIXED_TWO_PI = 0x0006487F; 
            const int32_t PHASE_G_OFFSET = 0x0002182A;
            const int32_t PHASE_B_OFFSET = 0x00043054;
            const int CYCLES = 2; 
            for (int j = 0; j < 60; j++) {
                int32_t base_phase = (FIXED_TWO_PI * CYCLES * j) / 60;
                int32_t phase_r = base_phase;
                int32_t phase_g = base_phase + PHASE_G_OFFSET;
                int32_t phase_b = base_phase + PHASE_B_OFFSET;
                int32_t sin_r = fast_sin_fixed(phase_r);
                int32_t sin_g = fast_sin_fixed(phase_g);
                int32_t sin_b = fast_sin_fixed(phase_b);
                int r = ((sin_r * 127) / FIXED_ONE) + 128;
                int g = ((sin_g * 127) / FIXED_ONE) + 128;
                int b = ((sin_b * 127) / FIXED_ONE) + 128;
                if (r < 0) r = 0; if (r > 255) r = 255;
                if (g < 0) g = 0; if (g > 255) g = 255;
                if (b < 0) b = 0; if (b > 255) b = 255;
                printf("38;2;%d;%d;%d ", r, g, b);
            } 
            printf("\n"); 
            return 0;
        }

        if (!strcmp(arg,"--preset") && i+1 < argc) {
            i++;
            if (!strcmp(argv[i],"cyberpunk")) { 
                anim_mode = 0;
                speed_fixed = FLOAT_TO_FIXED(0.3);
                freq_fixed_local = FLOAT_TO_FIXED(0.5);
                gradient_angle_fixed = FLOAT_TO_FIXED(45.0);
            } else if (!strcmp(argv[i],"retro")) { 
                anim_mode = 4;
                speed_fixed = FLOAT_TO_FIXED(0.2);
                freq_fixed_local = FLOAT_TO_FIXED(0.8);
                gradient_angle_fixed = FLOAT_TO_FIXED(0.0);
            } else if (!strcmp(argv[i],"matrix")) { 
                anim_mode = 10;
                speed_fixed = FLOAT_TO_FIXED(0.5);
                freq_fixed_local = FLOAT_TO_FIXED(1.2);
                gradient_angle_fixed = FLOAT_TO_FIXED(90.0);
            } else if (!strcmp(argv[i],"sunset")) { 
                anim_mode = 1;
                speed_fixed = FLOAT_TO_FIXED(0.15);
                freq_fixed_local = FLOAT_TO_FIXED(0.3);
                gradient_angle_fixed = FLOAT_TO_FIXED(30.0);
            }
            freq_fixed = freq_fixed_local;
            precalc_gradient_angle();
            continue;
        }
        
        bool is_numeric_flag = (!strcmp(arg,"-d") || !strcmp(arg,"-s") || !strcmp(arg,"-f") || 
                                !strcmp(arg,"-m") || !strcmp(arg,"-A") || !strcmp(arg,"-c") || 
                                !strcmp(arg,"-o") || !strcmp(arg,"-p") || !strcmp(arg,"-F"));

        if (is_numeric_flag) {
            if (i + 1 >= argc || argv[i+1][0] == '-') {
                fprintf(stderr, MSG(MSG_ERR_MISSING_VALUE), arg);
                return 1;
            }
            char *val = argv[i+1];
            int32_t num_fixed = str_to_fixed(val);
            if (!strcmp(arg,"-d")) { 
                duration_fixed = num_fixed;
                duration_us = ((uint64_t)duration_fixed * 1000000) / FIXED_ONE;
            } else if (!strcmp(arg,"-s")) {
                speed_fixed = num_fixed;
            } else if (!strcmp(arg,"-f")) { 
                freq_fixed_local = num_fixed;
                freq_fixed = freq_fixed_local;
            } else if (!strcmp(arg,"-m")) { 
                int tmp = num_fixed / FIXED_ONE;
                if (tmp >= 0 && tmp <= 11) anim_mode = tmp; 
                else { 
                    fprintf(stderr, "%s", MSG(MSG_ERR_MODE_LIMIT)); 
                    return 1; 
                }
            } else if (!strcmp(arg,"-c")) { 
                fixed_width = num_fixed / FIXED_ONE;
            } else if (!strcmp(arg,"-A")) { 
                gradient_angle_fixed = num_fixed;
                precalc_gradient_angle();
            } else if (!strcmp(arg,"-o")) { 
                char *o_val = argv[i+1];
                int32_t int_part = 0;
                int32_t frac_part = 0;
                int frac_len = 0;
                if (*o_val == '-') o_val++;
                while (*o_val >= '0' && *o_val <= '9') {
                    int_part = int_part * 10 + (*o_val - '0');
                    o_val++;
                }
                if (*o_val == '.') {
                    o_val++;
                    while (*o_val >= '0' && *o_val <= '9' && frac_len < 3) {
                        frac_part = frac_part * 10 + (*o_val - '0');
                        frac_len++;
                        o_val++;
                    }
                }
                while (frac_len < 3) {
                    frac_part *= 10;
                    frac_len++;
                }
                opacity_fixed = (int_part * 1000) + frac_part;
                if (opacity_fixed < 0) opacity_fixed = 0;
                if (opacity_fixed > 1000) opacity_fixed = 1000;
            } else if (!strcmp(arg,"-p")) { 
                start_phase_fixed = num_fixed;
            } else if (!strcmp(arg,"-F")) { 
                if (num_fixed > 0) {
                    int32_t fps_fixed = num_fixed;
                    frame_time_us = (uint32_t)((1000000LL * FIXED_ONE) / fps_fixed);
                }
            }
            i++;
            continue;
        }
        
        fprintf(stderr, MSG(MSG_ERR_INVALID_OPTION), arg);
        show_help();
        return 1;
    }
    
    precalc_gradient_angle();
    if (start_phase_fixed >= 0) {
        phase_fixed = start_phase_fixed;
    } else {
        const int32_t TWO_PI_FIXED = 411774;
        phase_fixed = (int32_t)(((uint64_t)rand() * TWO_PI_FIXED) / RAND_MAX);
    }
    
    wchar_t buf[MAX_LINE_LEN];
    content.count = 0;
    int max_w = 0;
    size_t buf_size = 0;
    char *frame_buf = NULL;

    if (stream_mode) {
        goto SKIP_BUFFER_LOADING;
    }

    while (fgetws(buf, MAX_LINE_LEN, stdin)) {
        if (content.count >= (MAX_LINES - 1)) {
            fprintf(stderr, "%s", MSG(MSG_ERR_LEN_LIMIT));
            sleep_us(2000000); 
            
            stream_mode = true;
            break; 
        }

        size_t len = wcslen(buf);
        if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
        for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; }
        
        content.lines[content.count++] = wcsdup(buf);
    }

SKIP_BUFFER_LOADING:

    if (stream_mode) {
        int line_count = content.count;
        write(STDOUT_FILENO, "\033[?7l\033[?25l", 11);
        int exit_status = 0;
        precalc_gradient_angle();
        int32_t phase = phase_fixed;

        for (int i = 0; i < line_count; i++) {
            wchar_t *line_buf = content.lines[i];
            size_t len = wcslen(line_buf);
            int32_t y_fixed = FLOAT_TO_FIXED(i);
            int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
            int32_t cx_fixed = FLOAT_TO_FIXED(len / 2.0f);
            int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
            
            for (size_t x = 0; x < len; x++) {
                int r, g, b;
                int32_t x_fixed = FLOAT_TO_FIXED(x);
                get_color_fast(x_fixed, y_fixed, anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
                printf("\033[38;2;%d;%d;%dm%lc", r, g, b, line_buf[x]);
            }
            printf("\033[0m\n");
            fflush(stdout);
            phase += speed_fixed;
            free(content.lines[i]); 
        }
        content.count = 0;

        while (fgetws(buf, MAX_LINE_LEN, stdin)) {
            size_t len = wcslen(buf);
            if (len > 0 && buf[len-1] == L'\n') buf[len-1] = L'\0';
            for (size_t k = 0; k < len; k++) { if (buf[k] == 0x1B) buf[k] = L'?'; }
            
            int32_t y_fixed = FLOAT_TO_FIXED(line_count);
            int32_t cy_fixed = FLOAT_TO_FIXED(0.5f);
            int32_t cx_fixed = FLOAT_TO_FIXED(len / 2.0f);
            int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
            
            for (size_t x = 0; x < len; x++) {
                int r, g, b;
                int32_t x_fixed = FLOAT_TO_FIXED(x);
                get_color_fast(x_fixed, y_fixed, anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
                printf("\033[38;2;%d;%d;%dm%lc", r, g, b, buf[x]);
            }
            printf("\033[0m\n");
            fflush(stdout);
            phase += speed_fixed;
            line_count++;
            if (ferror(stdin)) {
                exit_status = 1;
                break;
            }
        }
        write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16);
        return exit_status;
    }

    if (content.count == 0) return 0;
    for(int i=0; i<content.count; i++) { 
        int l = wcslen(content.lines[i]); 
        if(l>max_w) max_w=l;
    }
    if (fixed_width > 0) max_w = fixed_width;
    
    buf_size = content.count * (max_w * 32 + 100);
    frame_buf = malloc(buf_size);
    if (!frame_buf) {
        free_content(&content);
        return 1;
    }
    
    #define SAFE_APPEND(...) do { \
        size_t rem = buf_size - (ptr - frame_buf); \
        int n = snprintf(ptr, rem, __VA_ARGS__); \
        if (n > 0 && (size_t)n < rem) ptr += n; \
    } while(0)
    
    int32_t phase = phase_fixed;
    write(STDOUT_FILENO, "\033[?25l\033[?7l", 11);
    bool first_frame = true;
    precalc_gradient_angle();
    int32_t cx_fixed = FLOAT_TO_FIXED(max_w / 2.0f);
    int32_t cy_fixed = FLOAT_TO_FIXED(content.count / 2.0f);
    int32_t max_dist_fixed = fast_dist_fixed(cx_fixed, cy_fixed);
    uint64_t start_time_us = get_time_us();

    while (1) {
        if (duration_us > 0) {
            uint64_t now_us = get_time_us();
            if (now_us - start_time_us >= duration_us) break;
        }
        char *ptr = frame_buf;
        SAFE_APPEND("\033[?2026h");
        if (!first_frame) { SAFE_APPEND("\033[%dA", content.count); }
        first_frame = false;
        for (int y = 0; y < content.count; y++) {
            wchar_t *line = content.lines[y];
            int line_len = wcslen(line);
            int32_t y_fixed = FLOAT_TO_FIXED(y);
            SAFE_APPEND("\r");
            int last_r = -1, last_g = -1, last_b = -1;
            
            for (int x = 0; x < line_len; x++) {
                int r, g, b;
                int32_t x_fixed = FLOAT_TO_FIXED(x);
                get_color_fast(x_fixed, y_fixed, anim_mode, cx_fixed, cy_fixed, max_dist_fixed, phase, &r, &g, &b);
                
                if (line[x] == L' ') {
                    SAFE_APPEND(" ");
                    last_r = r; last_g = g; last_b = b;
                } else {
                    if (r != last_r || g != last_g || b != last_b) {
                        SAFE_APPEND("\033[38;2;%d;%d;%dm", r, g, b);
                        last_r = r;
                        last_g = g;
                        last_b = b;
                    }
                    SAFE_APPEND("%lc", line[x]);
                }
            }
            SAFE_APPEND("\033[0m\033[K\n");
        }
        SAFE_APPEND("\033[?2026l");
        write(STDOUT_FILENO, frame_buf, ptr - frame_buf);
        if (static_mode) break;
        phase += speed_fixed;
        sleep_us(frame_time_us);
    }
    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m", 15);
    free_content(&content);
    free(frame_buf);
    return 0;
}