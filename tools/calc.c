/*
 * NeonX — Terminal Shader Engine
 * Copyright (C) 2026  inrryoff
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

#define FIXED_SHIFT 16
#define FIXED_ONE   (1 << FIXED_SHIFT)

// ── helpers ────────────────────────────────────────────────────────────────

/* Imprime um quadrinho colorido (2 células ANSI true-color) */
static void print_swatch(int r, int g, int b) {
    printf("\033[48;2;%d;%d;%dm  \033[0m", r, g, b);
}

static int32_t str_to_fixed(const char *s) {
    int neg = 0;
    if (*s == '-') { neg = 1; s++; }

    int32_t int_part = 0;
    while (*s >= '0' && *s <= '9') { int_part = int_part * 10 + (*s - '0'); s++; }

    int32_t frac_part = 0, frac_div = 1;
    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9' && frac_div < 100000) {
            frac_part = frac_part * 10 + (*s - '0');
            frac_div  *= 10;
            s++;
        }
    }

    int32_t result = (int_part << FIXED_SHIFT) +
                     (int32_t)(((int64_t)frac_part * FIXED_ONE) / frac_div);
    return neg ? -result : result;
}

static int parse_hex_color(const char *hex, int *r, int *g, int *b) {
    if (!hex) return 0;
    if (hex[0] == '#') hex++;
    if (strlen(hex) != 6) return 0;
    for (int i = 0; i < 6; i++)
        if (!isxdigit((unsigned char)hex[i])) return 0;
    unsigned int val;
    if (sscanf(hex, "%x", &val) != 1) return 0;
    *r = (val >> 16) & 0xFF;
    *g = (val >> 8)  & 0xFF;
    *b =  val        & 0xFF;
    return 1;
}

// ── ponto fixo ─────────────────────────────────────────────────────────────

static void print_fixed(const char *s) {
    int32_t f = str_to_fixed(s);
    int32_t int_part  = f >> FIXED_SHIFT;
    int32_t frac_raw  = f & (FIXED_ONE - 1);
    int32_t frac_mils = (int32_t)(((int64_t)frac_raw * 1000) / FIXED_ONE);

    printf("\n  Ponto fixo Q16.16 para %s:\n\n", s);
    printf("    Decimal  : %d.%03d\n", int_part, frac_mils);
    printf("    Q16.16   : %d\n", f);
    printf("    Hex      : 0x%08X\n\n", (uint32_t)f);
}

// ── graus ──────────────────────────────────────────────────────────────────

static void print_degrees(const char *s) {
    int32_t deg_fixed = str_to_fixed(s);
    int32_t deg_int   = deg_fixed >> FIXED_SHIFT;
    int32_t angle_val = deg_int * FIXED_ONE;
    int32_t rad_fixed = (int32_t)(((int64_t)angle_val * 1143) >> FIXED_SHIFT);

    printf("\n  Conversão de %d graus:\n\n", deg_int);
    printf("    Campo angle no preset : %d\n", angle_val);
    printf("    Em radianos Q16.16    : %d\n", rad_fixed);
    printf("    Hex do angle          : 0x%08X\n\n", (uint32_t)angle_val);
    printf("    Uso no preset:\n");
    printf("    {\"meu_preset\", modo, speed, freq, %d, ...}\n\n", angle_val);
}

// ── paleta ─────────────────────────────────────────────────────────────────

typedef struct {
    const char *name;
    const char *family;
    int32_t off_r, off_g, off_b;
    const char *hex;
} PaletteEntry;

static const PaletteEntry palette_table[] = {
    // vermelhos
    {"red",         "red",    0,      137233, 274466, "#FF2020"},
    {"crimson",     "red",    0,      180000, 310000, "#DC143C"},
    {"coral",       "red",    0,      110000, 260000, "#FF6040"},

    // laranjas
    {"orange",      "orange", 0,      102944, 274466, "#FF8020"},
    {"amber",       "orange", 0,      90000,  280000, "#FFA000"},
    {"gold",        "orange", 0,      80000,  290000, "#FFD020"},

    // amarelos
    {"yellow",      "yellow", 0,      68829,  205887, "#FFFF20"},
    {"lime",        "yellow", 310000, 0,      180000, "#80FF20"},

    // verdes
    {"green",       "green",  274466, 0,      137233, "#20FF20"},
    {"teal",        "green",  240000, 0,      120000, "#20C0C0"},
    {"mint",        "green",  260000, 0,      100000, "#20FFA0"},

    // azuis
    {"cyan",        "blue",   205887, 0,      68829,  "#20FFFF"},
    {"sky",         "blue",   180000, 0,      80000,  "#40C0FF"},
    {"blue",        "blue",   137233, 274466, 0,      "#2020FF"},
    {"navy",        "blue",   150000, 300000, 0,      "#102080"},
    {"cobalt",      "blue",   160000, 290000, 10000,  "#2040D0"},

    // roxos
    {"purple",      "purple", 0,      274466, 137233, "#8020FF"},
    {"violet",      "purple", 0,      250000, 120000, "#6020FF"},
    {"indigo",      "purple", 60000,  274466, 80000,  "#4020C0"},
    {"lavender",    "purple", 20000,  240000, 100000, "#A080FF"},
    {"blue_purple", "purple", 80000,  274466, 60000,  "#6040FF"},

    // rosas / magentas
    {"magenta",     "pink",   0,      205887, 68829,  "#FF20FF"},
    {"pink",        "pink",   0,      180000, 320000, "#FF80C0"},
    {"hotpink",     "pink",   0,      200000, 290000, "#FF40A0"},
    {"rose",        "pink",   0,      160000, 300000, "#FF4080"},

    // neutros
    {"white",       "white",  0,      0,      0,      "#FFFFFF"},
};

#define NUM_PALETTE (sizeof(palette_table) / sizeof(palette_table[0]))

// ── Restaurada a função list_all_palettes ──────────────────────────────────
static void list_all_palettes(void) {
    const char *families[] = {"red","orange","yellow","green","blue","purple","pink","white"};
    int nf = 8;

    printf("\n  Cores disponíveis (use ./calc palets <família> para filtrar):\n\n");
    for (int f = 0; f < nf; f++) {
        printf("  ── %s ──\n", families[f]);
        for (size_t i = 0; i < NUM_PALETTE; i++) {
            if (strcmp(palette_table[i].family, families[f]) == 0) {
                int pr, pg, pb;
                parse_hex_color(palette_table[i].hex, &pr, &pg, &pb);
                printf("    ");
                print_swatch(pr, pg, pb);
                printf(" %-14s  %s   off_r=%-7d off_g=%-7d off_b=%d\n",
                       palette_table[i].name,
                       palette_table[i].hex,
                       palette_table[i].off_r,
                       palette_table[i].off_g,
                       palette_table[i].off_b);
            }
        }
        printf("\n");
    }
    printf("  Uso: ./calc palets <nome>    ex: ./calc palets blue_purple\n");
    printf("       ./calc hex \"#6A0DAD\"   para converter HEX direto\n\n");
}
// ───────────────────────────────────────────────────────────────────────────

static void print_palette(const char *color) {
    for (size_t i = 0; i < NUM_PALETTE; i++) {
        if (strcmp(color, palette_table[i].name) == 0) {
            printf("\n  Paleta senoidal para \"%s\" (%s):\n\n",
                   palette_table[i].name, palette_table[i].hex);
            printf("    off_r = %-10d   (0x%06X)\n", palette_table[i].off_r, palette_table[i].off_r);
            printf("    off_g = %-10d   (0x%06X)\n", palette_table[i].off_g, palette_table[i].off_g);
            printf("    off_b = %-10d   (0x%06X)\n\n", palette_table[i].off_b, palette_table[i].off_b);
            printf("    Uso no preset:\n");
            printf("    {\"meu_preset\", modo, speed, freq, angle, true, %d, %d, %d, false,0,0,0,0,0,0}\n\n",
                   palette_table[i].off_r, palette_table[i].off_g, palette_table[i].off_b);
            printf("  Espectro da família (%s):\n\n", palette_table[i].family);
            for (size_t j = 0; j < NUM_PALETTE; j++) {
                if (strcmp(palette_table[j].family, palette_table[i].family) == 0) {
                    int pr, pg, pb;
                    parse_hex_color(palette_table[j].hex, &pr, &pg, &pb);
                    printf("    ");
                    print_swatch(pr, pg, pb);
                    printf(" %s   off_r=%-7d off_g=%-7d off_b=%d\n",
                           palette_table[j].hex,
                           palette_table[j].off_r,
                           palette_table[j].off_g,
                           palette_table[j].off_b);
                }
            }
            printf("\n");
            return;
        }
    }

    int found = 0;
    for (size_t i = 0; i < NUM_PALETTE; i++) {
        if (strcmp(color, palette_table[i].family) == 0) {
            if (!found) {
                printf("\n  Espectro da família \"%s\":\n\n", color);
                found = 1;
            }
            int pr, pg, pb;
            parse_hex_color(palette_table[i].hex, &pr, &pg, &pb);
            printf("    ");
            print_swatch(pr, pg, pb);
            printf(" %s   off_r=%-7d off_g=%-7d off_b=%d\n",
                   palette_table[i].hex,
                   palette_table[i].off_r,
                   palette_table[i].off_g,
                   palette_table[i].off_b);
        }
    }
    if (found) { printf("\n"); return; }

    printf("\n  Cor \"%s\" não encontrada.\n", color);
    list_all_palettes();
}

// ── hex → preset ───────────────────────────────────────────────────────────

static void print_hex(const char *hex) {
    int r, g, b;
    if (!parse_hex_color(hex, &r, &g, &b)) {
        printf("\n  Formato inválido. Use: ./calc hex \"#RRGGBB\"\n\n");
        return;
    }

    printf("\n  HEX %s → RGB(%d, %d, %d)\n\n", hex, r, g, b);
    printf("  ── use_gradient (cor exata) ──────────────────────────────\n\n");
    printf("    gr=%d, gg=%d, gb=%d\n\n", r, g, b);
    printf("    Preset (cor1=%s, cor2=ajuste a segunda cor):\n", hex);
    printf("    {\"meu_preset\", modo, speed, freq, angle,\n");
    printf("     false,0,0,0, true, %d,%d,%d, R2,G2,B2}\n\n", r, g, b);

    size_t best = 0;
    int best_dist = INT32_MAX;
    for (size_t i = 0; i < NUM_PALETTE; i++) {
        int pr, pg, pb;
        parse_hex_color(palette_table[i].hex, &pr, &pg, &pb);
        int dr = r - pr, dg = g - pg, db = b - pb;
        int dist = dr*dr + dg*dg + db*db;
        if (dist < best_dist) { best_dist = dist; best = i; }
    }

    printf("  ── paleta senoidal mais próxima: \"%s\" (%s) ──────────────\n\n",
           palette_table[best].name, palette_table[best].hex);
    printf("    off_r = %d\n", palette_table[best].off_r);
    printf("    off_g = %d\n", palette_table[best].off_g);
    printf("    off_b = %d\n\n", palette_table[best].off_b);
    printf("    Preset:\n");
    printf("    {\"meu_preset\", modo, speed, freq, angle,\n");
    printf("     true, %d, %d, %d, false,0,0,0,0,0,0}\n\n",
           palette_table[best].off_r,
           palette_table[best].off_g,
           palette_table[best].off_b);
}

// ── ajuda ──────────────────────────────────────────────────────────────────

static void print_help(void) {
    printf("\n  calc — Calculadora de Ponto Fixo NeonX\n\n");
    printf("  Uso:\n\n");
    printf("    ./calc <valor>              → decimal para Q16.16\n");
    printf("    ./calc <graus> --graus      → graus para campo angle\n");
    printf("    ./calc palets               → lista todas as cores\n");
    printf("    ./calc palets <nome|família>→ detalhe + família da cor\n");
    printf("    ./calc hex \"#RRGGBB\"        → HEX para preset pronto\n\n");
    printf("  Exemplos:\n\n");
    printf("    ./calc 0.5\n");
    printf("    ./calc 120 --graus\n");
    printf("    ./calc palets blue\n");
    printf("    ./calc palets purple\n");
    printf("    ./calc hex \"#6A0DAD\"\n\n");
}

// ── gerador procedural de espectro ─────────────────────────────────────────

static int32_t calc_sine_offset(int color_val) {
    float normalized = (color_val - 128.0f) / 127.0f;
    if (normalized > 1.0f) normalized = 1.0f;
    if (normalized < -1.0f) normalized = -1.0f;
    float rads = acosf(normalized);
    return (int32_t)(rads * 65536.0f);
}

static void hsl_to_rgb(float h, float s, float l, int *r, int *g, int *b) {
    float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;
    float rf = 0, gf = 0, bf = 0;
    
    if (h < 60) { rf = c; gf = x; bf = 0; }
    else if (h < 120) { rf = x; gf = c; bf = 0; }
    else if (h < 180) { rf = 0; gf = c; bf = x; }
    else if (h < 240) { rf = 0; gf = x; bf = c; }
    else if (h < 300) { rf = x; gf = 0; bf = c; }
    else { rf = c; gf = 0; bf = x; }
    
    *r = (int)((rf + m) * 255.0f);
    *g = (int)((gf + m) * 255.0f);
    *b = (int)((bf + m) * 255.0f);
}

/* entrada gerada para ordenação */
typedef struct {
    int r, g, b;
    int32_t off_r, off_g, off_b;
    float luma; /* lightness HSL = (max+min)/2 normalizado */
} GenEntry;

static int cmp_luma(const void *a, const void *b) {
    const GenEntry *ea = (const GenEntry *)a;
    const GenEntry *eb = (const GenEntry *)b;
    if (ea->luma < eb->luma) return -1;
    if (ea->luma > eb->luma) return  1;
    return 0;
}

static void generate_spectrum(const char *family, int count) {
    float base_hue = 0.0f;

    if      (strcmp(family, "red")    == 0) base_hue =   0.0f;
    else if (strcmp(family, "orange") == 0) base_hue =  30.0f;
    else if (strcmp(family, "yellow") == 0) base_hue =  60.0f;
    else if (strcmp(family, "green")  == 0) base_hue = 120.0f;
    else if (strcmp(family, "cyan")   == 0) base_hue = 180.0f;
    else if (strcmp(family, "blue")   == 0) base_hue = 240.0f;
    else if (strcmp(family, "purple") == 0) base_hue = 280.0f;
    else if (strcmp(family, "pink")   == 0) base_hue = 320.0f;
    else {
        printf("\n  Família não suportada. Use: red, orange, yellow, green, cyan, blue, purple, pink.\n\n");
        return;
    }

    if (count < 1) count = 1;

    GenEntry *entries = (GenEntry *)malloc((size_t)count * sizeof(GenEntry));
    if (!entries) { printf("\n  Erro de memória.\n\n"); return; }

    for (int i = 0; i < count; i++) {
        /* hue FIXO — só lightness varia de 0.08 (preto) a 0.88 (branco) */
        float l = 0.08f + 0.80f * (float)i / (float)(count > 1 ? count - 1 : 1);
        /* saturação alta e fixa — garante que a cor seja sempre reconhecível */
        float s = 0.90f;
        /* nas extremidades muito escuras/claras, satura menos pra não virar cinza */
        if (l < 0.15f) s = 0.70f + 0.20f * (l / 0.15f);
        if (l > 0.75f) s = 0.90f - 0.50f * ((l - 0.75f) / 0.13f);
        if (s < 0.0f) s = 0.0f;
        if (s > 1.0f) s = 1.0f;

        int r, g, b;
        hsl_to_rgb(base_hue, s, l, &r, &g, &b);

        entries[i].r     = r;
        entries[i].g     = g;
        entries[i].b     = b;
        entries[i].off_r = calc_sine_offset(r);
        entries[i].off_g = calc_sine_offset(g);
        entries[i].off_b = calc_sine_offset(b);
        /* lightness HSL: (max+min)/2 */
        int mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
        int mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
        entries[i].luma = (mx + mn) / (2.0f * 255.0f);
    }

    /* ordena do mais escuro ao mais claro (já deve estar em ordem, mas garante) */
    qsort(entries, (size_t)count, sizeof(GenEntry), cmp_luma);

    printf("\n  Gerando %d variações calculadas para a família \"%s\" (escuro → claro):\n\n",
           count, family);

    for (int i = 0; i < count; i++) {
        printf("    ");
        print_swatch(entries[i].r, entries[i].g, entries[i].b);
        printf(" #%02X%02X%02X   off_r=%-7d off_g=%-7d off_b=%d\n",
               entries[i].r, entries[i].g, entries[i].b,
               entries[i].off_r, entries[i].off_g, entries[i].off_b);
    }
    printf("\n");
    free(entries);
}

// ── main ───────────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {
    if (argc < 2) { print_help(); return 0; }

    if (strcmp(argv[1], "palets") == 0) {
        if (argc < 3) { list_all_palettes(); return 0; }
        print_palette(argv[2]);
        return 0;
    }

    if (strcmp(argv[1], "gen") == 0) {
        if (argc < 3) {
            printf("\n  Uso: ./calc gen <familia> [quantidade]\n");
            printf("  Ex : ./calc gen blue 50\n\n");
            return 1;
        }
        int count = 20;
        if (argc >= 4) count = atoi(argv[3]);
        
        generate_spectrum(argv[2], count);
        return 0;
    }

    if (strcmp(argv[1], "hex") == 0) {
        if (argc < 3) {
            printf("\n  Uso: ./calc hex \"#RRGGBB\"\n\n");
            return 1;
        }
        print_hex(argv[2]);
        return 0;
    }

    if (argc >= 3 && strcmp(argv[2], "--graus") == 0) {
        print_degrees(argv[1]);
        return 0;
    }

    print_fixed(argv[1]);
    return 0;
}
