#include "terminal.h"
#include "msgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

Content content = {.count = 0};
static int g_integrity_status = 2;

const char* ORIGINAL_CREATOR = "@inrryoff";

#ifndef BUILD_MAINTAINER
    #define BUILD_MAINTAINER "@inrryoff"
#endif
#ifndef VERSION
    #define VERSION "2.0.3-LAB"
#endif

void set_integrity_status(int status) {
    g_integrity_status = status;
}

void sleep_us(double microseconds) {
    usleep((useconds_t)microseconds);
}

void free_content(Content *c) {
    for(int i=0; i<c->count; i++) if(c->lines[i]) free(c->lines[i]);
    c->count = 0;
}

void print_version(void) {
    if (g_integrity_status == 0) {
        printf("NeonX v%s\n", VERSION);
        printf(MSG(MSG_VERSION_ORIGINAL_CREATOR), ORIGINAL_CREATOR);
        printf(MSG(MSG_VERSION_COMPILED_BY), BUILD_MAINTAINER);
        printf("%s", MSG(MSG_VERSION_STATUS_OFFICIAL));
    } else {
        printf("%s", MSG(MSG_VERSION_MODIFIED));
        printf(MSG(MSG_VERSION_ORIGINAL_CREATOR), MSG(MSG_UNKNOWN));
        printf(MSG(MSG_VERSION_COMPILED_BY), MSG(MSG_UNKNOWN));
        printf("%s", MSG(MSG_VERSION_STATUS_MODIFIED));
    }
}

void print_license(void) {
    printf("%s", MSG(MSG_LICENSE_TEXT));
}

void show_help(void) {
    printf(MSG(MSG_HELP_HEADER), VERSION, ORIGINAL_CREATOR, BUILD_MAINTAINER);
    printf("%s", MSG(MSG_HELP_USAGE));
    printf("%s", MSG(MSG_HELP_M));
    printf("%s", MSG(MSG_HELP_S));
    printf("%s", MSG(MSG_HELP_F));
    printf("%s", MSG(MSG_HELP_D));
    printf("%s", MSG(MSG_HELP_A));
    printf("%s", MSG(MSG_HELP_P));
    printf("%s", MSG(MSG_HELP_S_UPPER));
    printf("%s", MSG(MSG_HELP_C));
    printf("%s", MSG(MSG_HELP_O));
    printf("%s", MSG(MSG_HELP_F_UPPER));
    printf("%s", MSG(MSG_HELP_L));
    printf("%s", MSG(MSG_HELP_PRESET));
    printf("%s", MSG(MSG_HELP_QUANTIZED));
    printf("%s", MSG(MSG_HELP_SPIN));
    printf("%s", MSG(MSG_HELP_LANG));
    printf("%s", MSG(MSG_HELP_LICENSE));
    printf("%s", MSG(MSG_HELP_VERSION));
    printf("%s", MSG(MSG_HELP_HELP));
}

void handle_sigint(int sig) {
    write(STDOUT_FILENO, "\033[?7h\033[?25h\033[0m\n", 16);
    free_content(&content);
    exit(0);
}
