#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <wchar.h>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

#include "integrity.h"
#include "shaders.h"
#include "terminal.h"

typedef struct { int r, g, b; } RGBColor;

static bool has_flag(int argc, char *argv[], const char *flag) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) return true;
    }
    return false;
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    setup_terminal();
    init_lut();
    srand((unsigned int)time(NULL)); 
    signal(SIGINT, handle_sigint);
    
    int auth_status = check_integrity();
    set_integrity_status(auth_status);
    if (auth_status == 2) {
        if (!has_flag(argc, argv, "--allow-mod")) {
            sleep_us(1500000);
        }
    }

    bool static_mode = false, stream_mode = false;
    int anim_mode = 0;
    double speed = 0.2, duration = 0;
    double start_phase = -1.0;
    int fixed_width = 0;
    double frame_time_us = 50000;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (!strcmp(arg,"-h") || !strcmp(arg,"--help")) { show_help(); return 0; }
        if (!strcmp(arg,"-v") || !strcmp(arg,"--version")) { print_version(); return 0; }
        if (!strcmp(arg,"--license")) { print_license(); return 0; }
        if (!strcmp(arg,"-S")) { static_mode = true; continue; }
        if (!strcmp(arg,"-L")) { stream_mode = true; continue; }
        if (!strcmp(arg,"--allow-mod")) { continue; }

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
        printf("\033[?7l\033[?25l"); fflush(stdout);
        
        while (fgetws(buffer, MAX_LINE_LEN, stdin)) {
            size_t len = wcslen(buffer);
            if (len > 0 && buffer[len-1] == L'\n') buffer[len-1] = L'\0';
            for (size_t k = 0; k < len; k++) { if (buffer[k] == 0x1B) buffer[k] = L'?'; }
            
            for (size_t x = 0; x < len; x++) {
                int r,g,b;
                get_color_fast((int)x, line_count, anim_mode, (int)len, 1, phase, &r, &g, &b);
                printf("\033[38;2;%d;%d;%dm%lc", r, g, b, buffer[x]);
            }
            printf("\033[0m\n"); fflush(stdout);
            phase += speed;
            line_count++;
        }
        printf("\033[?7h\033[?25h\033[0m\n");
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
    printf("\033[?25l\033[?7l"); fflush(stdout);
    bool first_frame = true;
    
    while (1) {
        if (sigint_triggered()) {
            printf("\033[?7h\033[?25h\033[0m\n");
            free_content(&content);
            free(frame_buf);
            free(color_cache);
            return 0;
        }
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
        
        #ifdef _WIN32
            fwrite(frame_buf, 1, ptr - frame_buf, stdout); fflush(stdout);
        #else
            write(STDOUT_FILENO, frame_buf, ptr - frame_buf);
        #endif
        
        if (static_mode) break;

        phase += speed;
        sleep_us((long)frame_time_us);
    }

    printf("\033[?7h\033[?25h\033[0m"); fflush(stdout);
    free_content(&content);
    free(frame_buf);
    free(color_cache);
    return 0;
}
