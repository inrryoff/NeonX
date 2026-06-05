#ifndef STYLE_H
#define STYLE_H

/* ==================== RESET GERAL ==================== */
#define RESET     "\033[0m"

/* ==================== CORES DE TEXTO (NORMAL) ==================== */
#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define WHITE     "\033[0;37m"

/* ==================== CORES BRILHANTES (NEGRITO + COR) ==================== */
#define BRIGHT_BLACK   "\033[1;30m"
#define BRIGHT_RED     "\033[1;31m"
#define BRIGHT_GREEN   "\033[1;32m"
#define BRIGHT_YELLOW  "\033[1;33m"
#define BRIGHT_BLUE    "\033[1;34m"
#define BRIGHT_MAGENTA "\033[1;35m"
#define BRIGHT_CYAN    "\033[1;36m"
#define BRIGHT_WHITE   "\033[1;37m"

/* ==================== FUNDOS SÓLIDOS ==================== */
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"
#define BG_FOSCO   "\033[48;5;237m\033[38;5;253m" 
#define BG_GRAY    BG_WHITE

/* ==================== FUNDOS COM TEXTO (COMBINAÇÕES ÚTEIS) ==================== */
#define HEADER_SECTION   BG_GRAY BRIGHT_BLACK
#define BG_RED_BOLD      BG_RED BRIGHT_WHITE
#define BG_YELLOW_BOLD   BG_YELLOW BRIGHT_BLACK

/* ==================== EFEITOS ==================== */
#define BOLD       "\033[1m"
#define DIM        "\033[2m"
#define ITALIC     "\033[3m"
#define UNDERLINE  "\033[4m"
#define BLINK      "\033[5m"
#define REVERSE    "\033[7m"
#define HIDDEN     "\033[8m"
#define STRIKE     "\033[9m"

/* ==================== COMBINAÇÕES PRONTAS PARA O NEONX ==================== */
#define MSG_SUCESSO      BRIGHT_GREEN
#define MSG_ERRO         BRIGHT_RED
#define MSG_AVISO        BRIGHT_YELLOW
#define MSG_INFO         BRIGHT_CYAN
#define MSG_DESTAQUE     BRIGHT_WHITE
#define MSG_SUBLINHA     UNDERLINE CYAN
#define MSG_FUNDO_CINZA  BG_GRAY BRIGHT_BLACK
#define MSG_HEADER       BOLD BRIGHT_MAGENTA
#define MSG_CLAUSE_NUM   BG_GRAY BRIGHT_BLACK
#define MSG_FILEPATH     UNDERLINE CYAN
#define MSG_COMMAND      BRIGHT_GREEN
#define MSG_NUMBER       BRIGHT_YELLOW
#define MSG_ICON_SUCCESS BRIGHT_GREEN
#define MSG_ICON_ERROR   BRIGHT_RED
#define MSG_ICON_WARN    BRIGHT_YELLOW
#define MSG_ICON_INFO    BRIGHT_CYAN
#define MSG_LEGAL_TXT  ITALIC DIM UNDERLINE
#define MSG_CMD_DIM    DIM
#define MSG_URL          ITALIC UNDERLINE BRIGHT_BLUE
#define LOGO_NEONX BRIGHT_RED "N" BRIGHT_YELLOW "e" BRIGHT_GREEN "o" BRIGHT_CYAN "n" BRIGHT_BLUE "X" RESET
#define LICENSE_LOGO BRIGHT_RED "N" BRIGHT_YELLOW "E" BRIGHT_GREEN "O" BRIGHT_CYAN "N" BRIGHT_BLUE "X" RESET

#endif