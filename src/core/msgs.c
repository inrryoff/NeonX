#include "neonx.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/** 
 * Returns the virtual offset for localized text alignment tables.
 */
uint32_t nx_msgs_get_locale_voffset(void) {
    /* Alignment component C: fragmented for UTF-8 table mapping */
    uint32_t c1 = NX_FRAGMENT_C ^ 0x55555555UL;
    uint32_t c2 = 0x55555555UL;
    return c1 ^ c2;
}

static char g_strip_buf[2][8192];
static int g_strip_idx = 0;

static const char* strip_ansi(const char *src) {
    if (!src) return "";
    char *dest = g_strip_buf[g_strip_idx];
    g_strip_idx = (g_strip_idx + 1) % 2;
    char *d = dest;
    const char *s = src;
    while (*s && (size_t)(d - dest) < sizeof(g_strip_buf[0]) - 1) {
        if (*s == '\033' && *(s + 1) == '[') {
            s += 2;
            while (*s && !((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z'))) s++;
            if (*s) s++;
        } else {
            *d++ = *s++;
        }
    }
    *d = '\0';
    return dest;
}

const char* MSG_F(enum Mensagem id, bool disable_ansi) {
    const char *msg = get_msg(id);
    if (disable_ansi) {
        return strip_ansi(msg);
    }
    return msg;
}

#ifdef _WIN32
#include <windows.h>
#endif

static int idioma_atual = 1;
static const char *lang_prefixes[19] = {
    "pt", // 0: Português
    "en", // 1: Inglês
    "es", // 2: Espanhol
    "fr", // 3: Francês
    "de", // 4: Alemão
    "it", // 5: Italiano
    "ru", // 6: Russo
    "zh", // 7: Chinês
    "ja", // 8: Japonês
    "ko", // 9: Coreano
    "tr", // 10: Turco
    "pl", // 11: Polonês
    "id", // 12: Indonésio
    "ar", // 13: Árabe
    "bg", // 14: Búlgaro
    "el", // 15: Grego
    "hi", // 16: Hindi
    "th", // 17: Tailandês
    "km"  // 18: Khmer
};

// =========================================================================
// MACROS DE LICENÇA (Evita inflar o tamanho do binário compilado)
// =========================================================================
#define LICENSE_PT \
    "LICENÇA DE USO - NEONX (C - VERSION)\n" \
    "-----------------------------------------------------------------\n" \
    "Copyright (c) 2026 @inrry\x6f\x66\x66 - Licenciado sob condições especiais " LOGO_NEONX " LICENSE\n\n" \
    "Pelo presente, fica concedida permissão a qualquer pessoa que obtenha uma cópia\n" \
    "deste software para usá-lo gratuitamente, sujeito às seguintes condições:\n\n" \
    MSG_LEGAL_TXT "1. ATRIBUIÇÃO (CRÉDITOS):" RESET "\n" \
    "   O nome do autor original (@inrry\x6f\x66\x66) and os avisos de copyright devem ser\n" \
    "   mantidos em todos os arquivos de código-fonte, cabeçalhos e na saída de\n" \
    "   versão do binário compilado (ex: " BG_FOSCO " neonx --version " RESET ").\n\n" \
    MSG_LEGAL_TXT "2. PROIBIÇÃO DE COMERCIALIZAÇÃO:" RESET "\n" \
    "   É TERMINANTEMENTE PROIBIDA a venda, aluguel ou qualquer forma de\n" \
    "   comercialização deste software, seja do código-fonte ou do binário\n" \
    "   compilado, de forma isolada ou integrada a pacotes pagos.\n\n" \
    MSG_LEGAL_TXT "3. DERIVAÇÕES E MODIFICAÇÕES:" RESET "\n" \
    "   Alterações no código são permitidas para melhorias ou uso pessoal, desde que:\n" \
    "   a) O trabalho derivado NÃO seja vendido.\n" \
    "   b) A versão modificada seja mantida em repositório público (Open Source).\n" \
    "   c) Os créditos ao autor original sejam mantidos de forma clara.\n\n" \
    MSG_LEGAL_TXT "4. DISTRIBUIÇÃO EM MÓDULOS (MAGISK/KERNELSU):" RESET "\n" \
    "   O uso deste binário em módulos de otimização é permitido e encorajado,\n" \
    "   desde que o módulo seja distribuído gratuitamente.\n\n" \
    MSG_CMD_DIM "O SOFTWARE É FORNECIDO 'COMO ESTÁ', SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU\n" \
    "IMPLÍCITA. EM NENHUM EVENTO O AUTOR SERÁ RESPONSÁVEL POR QUALQUER RECLAMAÇÃO,\n" \
    "DANOS OU OUTRA RESPONSABILIDADE RESULTANTE DO USO DESTE SOFTWARE." RESET "\n"

#define LICENSE_EN \
    "NEONX USE LICENSE (C - VERSION)\n" \
    "-----------------------------------------------------------------\n" \
    "Copyright (c) 2026 @inrry\x6f\x66\x66 - Licensed under special conditions " LOGO_NEONX " LICENSE\n\n" \
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n" \
    "of this software and associated documentation files, subject to the following:\n\n" \
    MSG_LEGAL_TXT "1. ATTRIBUTION (CREDITS):" RESET "\n" \
    "   The original author's name (@inrry\x6f\x66\x66) and copyright notices must be\n" \
    "   kept in all source files, headers, and compiled binary version outputs\n" \
    "   (e.g., " BG_FOSCO " neonx --version " RESET ").\n\n" \
    MSG_LEGAL_TXT "2. PROHIBITION OF COMMERCIALIZATION:" RESET "\n" \
    "   The sale, rental, or commercialization of this software is STRICTLY\n" \
    "   PROHIBITED, whether standalone or integrated into paid packages.\n\n" \
    MSG_LEGAL_TXT "3. DERIVATIONS AND MODIFICATIONS:" RESET "\n" \
    "   Modifications are allowed for personal use or improvement, provided:\n" \
    "   a) The derivative work is NOT sold.\n" \
    "   b) The modified version is open-sourced in a public repository.\n" \
    "   c) Original author credits are visibly maintained.\n\n" \
    MSG_LEGAL_TXT "4. MODULE DISTRIBUTION (MAGISK/KERNELSU):" RESET "\n" \
    "   Using this binary in optimization modules is allowed and encouraged,\n" \
    "   provided the module is distributed for free.\n\n" \
    MSG_CMD_DIM "THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND." RESET "\n"

// =========================================================================
// MATRIZ INTERNACIONAL (19 Idiomas)
// Índices: 0:PT, 1:EN, 2:ES, 3:FR, 4:DE, 5:IT, 6:RU, 7:ZH, 8:JA, 9:KO, 10:TR, 11:PL, 12:ID
// =========================================================================
static const char *mensagens[19][MSG_TOTAL] = {
    // ---------------- [0] PORTUGUÊS (PT) ----------------
    {
        MSG_ERRO "Erro ao abrir arquivo\n" RESET,
        "Criador Original: ",
        "Compilado por: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFICADO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_PT,
        LOGO_NEONX DIM ITALIC " - Embelezador de Terminal Avançado" RESET "\n",
        "Uso: " BG_FOSCO " cat arquivo | neonx [opcoes] " RESET "\n\n"
        "  -m [0-11]          Define o estilo da animação\n"
        "  -s [valor]         Velocidade da transição " MSG_CMD_DIM "(Padrão: 0.2)" RESET "\n"
        "  -f [valor]         Frequência da onda " MSG_CMD_DIM "(Padrão: 0.3)" RESET "\n"
        "  -d [valor]         Duração em segundos " MSG_CMD_DIM "(0 = Infinito)" RESET "\n"
        "  -max-lines [val]   Limite máximo de linhas " MSG_CMD_DIM "(Padrão: 10.000)" RESET "\n"
        "  -A [graus]         Rotaciona o ângulo do gradiente " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [valor]     Define uma seed fixa " MSG_CMD_DIM "(Determinística)" RESET "\n"
        "  -S                 Renderiza um quadro estático " MSG_CMD_DIM "(Sem animação)" RESET "\n"
        "  -c [largura]       Força uma largura estática para o gradiente\n"
        "  -o [0-1]           Ajusta a opacidade horizontal/suavidade\n"
        "  -O [0-1]           Ativa a Opacidade Vertical (fading topo/base)\n"
        "  -F [valor]         Trava a taxa de quadros " MSG_CMD_DIM "(ex: 60, 90)" RESET "\n"
        "  -L                 Processamento linha por linha " MSG_CMD_DIM "(Stream)" RESET "\n"
        "  --fo [0-1]         Ativa o Modo Fosco (Reduz vivacidade)\n"
        "  --preset [nome]    Carrega paletas " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias para --color1 e --color2\n"
        "  --color1 [hex]     Define a cor inicial do gradiente (ex: #FF0000)\n"
        "  --color2 [hex]     Define a cor final do gradiente (ex: #FFA500)\n"
        "  --quantized        Quantização de cores " MSG_CMD_DIM "(Maior performance)" RESET "\n"
        "  --no-ansi          Desativa o uso de cores ANSI na saída\n"
        "  --spin             Gera códigos ANSI puros " MSG_CMD_DIM "(Para scripts)" RESET "\n"
        "  --lang [id]        Sobrescreve o idioma " MSG_CMD_DIM "(ex: pt, en)" RESET "\n"
        "  --license          Exibe os termos de licenciamento\n"
        "  -v, --version      Mostra a versão e status do binário\n"
        "  -h, --help         Exibe este painel de ajuda interativo\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " A opção '%s' requer um valor numérico após ela.\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " A opção '%s' requer um valor numérico. Recebido: '%s'\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " O modo de animação (-m) deve ser um número inteiro.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " O modo de animação (-m) deve estar entre 0 e 11.\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " Opção inválida ou argumento solto '%s'\n",
        "\n" MSG_ERRO "[ ❌ ERRO 404 ]" RESET " Nenhum dado foi passado para o binário!\n",
        "\n" MSG_ERRO "[ ❌ ERRO 413 ]" RESET " Arquivo muito grande. Use -L para o modo stream.\n",
        MSG_AVISO "[ ⚠️ AVISO 403 ]" RESET " Integridade não verificada (sistema restrito/binário adulterado).\n",
        MSG_ERRO "[ ❌ ERRO 403 ]" RESET " Falha ao abrir o executável para verificação de integridade.\n",
        MSG_ERRO "[ ❌ ERRO 403 ]" RESET " Erro de leitura durante a verificação de integridade.\n",
        MSG_ERRO "[ ❌ ERRO 403 ]" RESET " Memória insuficiente para verificar a integridade.\n",
        MSG_ERRO "[ ❌ ERRO 403 ]" RESET " Assinatura inválida ou formato hexadecimal incorreto.\n",
        MSG_ERRO "[ ❌ ERRO 403 ]" RESET " O arquivo é muito pequeno para conter a assinatura.\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " A opção '%s' requer um número inteiro.\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " A opção '%s' requer um valor positivo.\n",
        MSG_ERRO "[ ❌ ERRO 400 ]" RESET " A duração não pode ser negativa.\n",
        MSG_ERRO "[ ❌ ERRO 206 ]" RESET " Não foi possível alocar memória (wcsdup).\n",
        MSG_AVISO "[ ⚠️ AVISO 206 ]" RESET " Buffer truncado para 32MB. Renderização pode falhar.\n",
        MSG_AVISO "[ ⚠️ AVISO 203 ]" RESET " Falha ao carregar chave customizada, usando embutida.\n",
        MSG_CMD_DIM "[ ⚙️ DEBUG 214 ]" RESET " Desalinhamento geométrico detectado. Estabilização pode variar.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [1] INGLÊS (EN) ----------------
    {
        MSG_ERRO "Error opening file\n" RESET,
        "Original Creator: ",
        "Compiled by: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFIED" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Advanced Terminal Beautifier" RESET "\n",
        "Usage: " BG_FOSCO " cat file | neonx [options] " RESET "\n\n"
        "  -m [0-11]          Defines the animation style\n"
        "  -s [val]           Transition speed " MSG_CMD_DIM "(Default: 0.2)" RESET "\n"
        "  -f [val]           Wave frequency " MSG_CMD_DIM "(Default: 0.3)" RESET "\n"
        "  -d [val]           Duration in seconds " MSG_CMD_DIM "(0 = Infinite)" RESET "\n"
        "  -max-lines [val]   Maximum lines limit " MSG_CMD_DIM "(Default: 10.000)" RESET "\n"
        "  -A [deg]           Rotates the gradient angle " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [val]       Sets a fixed seed " MSG_CMD_DIM "(Deterministic)" RESET "\n"
        "  -S                 Renders a static frame " MSG_CMD_DIM "(No animation)" RESET "\n"
        "  -c [width]         Forces a static gradient width\n"
        "  -o [0-1]           Adjusts horizontal opacity/smoothness\n"
        "  -O [0-1]           Enables Vertical Opacity (fading top/base)\n"
        "  -F [val]           Locks the framerate " MSG_CMD_DIM "(e.g., 60, 90)" RESET "\n"
        "  -L                 Line-by-line processing " MSG_CMD_DIM "(Stream)" RESET "\n"
        "  --fo [0-1]         Enables Matte Mode (Reduces vibrancy)\n"
        "  --preset [name]    Loads palettes " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias for --color1 and --color2\n"
        "  --color1 [hex]     Sets the starting color (e.g., #FF0000)\n"
        "  --color2 [hex]     Sets the ending color (e.g., #FFA500)\n"
        "  --quantized        Color quantization " MSG_CMD_DIM "(Higher performance)" RESET "\n"
        "  --no-ansi          Disables ANSI colors in output\n"
        "  --spin             Generates pure ANSI codes " MSG_CMD_DIM "(For scripts)" RESET "\n"
        "  --lang [id]        Overrides interface language " MSG_CMD_DIM "(e.g., pt, en)" RESET "\n"
        "  --license          Displays software licensing terms\n"
        "  -v, --version      Shows binary version and build status\n"
        "  -h, --help         Displays this interactive help panel\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " The '%s' option requires a numeric value after it.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " The '%s' option requires a numeric value. Received: '%s'\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " The animation mode (-m) must be an integer.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " The animation mode (-m) must be between 0 and 11.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Invalid option or loose argument '%s'\n",
        "\n" MSG_ERRO "[ ❌ ERROR 404 ]" RESET " No data was passed to the binary!\n",
        "\n" MSG_ERRO "[ ❌ ERROR 413 ]" RESET " File too large. Use -L for stream mode.\n",
        MSG_AVISO "[ ⚠️ WARN 403 ]" RESET " Could not verify integrity (restricted system/tampered binary).\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Failed to open the executable for integrity verification.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " File reading error during integrity verification.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Insufficient memory to verify integrity.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Invalid signature or incorrect hexadecimal format.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " The file is too small to contain the signature.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " The '%s' option requires an integer.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " The '%s' option requires a positive value.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Duration cannot be negative.\n",
        MSG_ERRO "[ ❌ ERROR 206 ]" RESET " Could not allocate memory (wcsdup).\n",
        MSG_AVISO "[ ⚠️ WARN 403 ]" RESET " Buffer truncated to 32MB. Rendering may corrupt.\n",
        MSG_AVISO "[ ⚠️ WARN 203 ]" RESET " Failed to load custom key, using built-in key.\n",
        MSG_CMD_DIM "[ ⚙️ DEBUG 214 ]" RESET " Geometric misalignment detected. Stabilization may vary.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    }
};

const char* get_msg(enum Mensagem id) {
    if (id < 0 || id >= MSG_TOTAL) return "";
    return mensagens[idioma_atual][id];
}

#ifdef _WIN32
const char* msgs_detect_windows_locale(void) {
    LANGID langId = GetUserDefaultUILanguage();
    (void)langId;
    wchar_t wlocaleName[LOCALE_NAME_MAX_LENGTH];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, wlocaleName, LOCALE_NAME_MAX_LENGTH) == 0) return "en";
    static char localeName[LOCALE_NAME_MAX_LENGTH];
    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, localeName, LOCALE_NAME_MAX_LENGTH, wlocaleName, _TRUNCATE);
    if (strlen(localeName) >= 2) localeName[2] = '\0';
    return localeName;
}
#endif

void msgs_set_language(const char *lang_code) {
    if (!lang_code || strlen(lang_code) < 2) {
        idioma_atual = 1;
        return;
    }

    char prefix[3];
    prefix[0] = (char)tolower((unsigned char)lang_code[0]);
    prefix[1] = (char)tolower((unsigned char)lang_code[1]);
    prefix[2] = '\0';

    int num_langs = sizeof(lang_prefixes) / sizeof(lang_prefixes[0]);
    
    for (int i = 0; i < num_langs; i++) {
        if (strncmp(prefix, lang_prefixes[i], 2) == 0) {
            idioma_atual = i;
            return;
        }
    }
    
    idioma_atual = 1; 
}

#include <locale.h>

void msgs_init(void) {
    const char *lang = NULL;
#ifdef _WIN32
    lang = msgs_detect_windows_locale();
#else
    lang = getenv("LC_ALL");
    if (!lang || !*lang) lang = getenv("LC_MESSAGES");
    if (!lang || !*lang) lang = getenv("LANG");
    if (!lang || !*lang) {
        char *loc = setlocale(LC_ALL, "");
        if (loc) lang = loc;
    }
#endif
    
    msgs_set_language(lang);
}
