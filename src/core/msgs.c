#include "neonx.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

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


#define LICENSE_PT \
    "LICENÇA DE SOFTWARE - " LICENSE_LOGO " (C - VERSION)\n" \
    "-------------------------------------------------------------------------------\n" \
    "Copyright (c) 2026 @inrry\x6f\x66\x66 - Licenciado sob condições especiais " LOGO_NEONX " LICENSE\n\n" \
    "É concedida permissão a qualquer pessoa que obtenha uma cópia deste software\n" \
    "para utilizá-lo gratuitamente, sujeito às seguintes condições:\n\n" \
    MSG_LEGAL_TXT "1. ATRIBUIÇÃO OBRIGATÓRIA:" RESET "\n" \
    "   O nome do autor original (@inrry\x6f\x66\x66) e os avisos de copyright devem ser\n" \
    "   preservados, de forma visível e legível, em:\n" \
    "   a) Todos os arquivos de código-fonte e cabeçalhos (.c, .h);\n" \
    "   b) Na saída do comando de versão do binário compilado\n" \
    "      (ex: " BG_FOSCO " neonx --version " RESET ");\n" \
    "   c) Em qualquer documentação pública ou README de trabalho derivado.\n" \
    "   A remoção ou ofuscação do nome do autor por qualquer meio\n" \
    "   constitui violação desta licença.\n\n" \
    MSG_LEGAL_TXT "2. PROIBIÇÃO DE COMERCIALIZAÇÃO:" RESET "\n" \
    "   É expressamente PROIBIDA qualquer forma de comercialização, incluindo:\n" \
    "   a) Venda direta do código-fonte ou do binário compilado;\n" \
    "   b) Aluguel, licenciamento pago ou assinatura de acesso ao Software;\n" \
    "   c) Inclusão em produtos ou pacotes pagos, mesmo como componente secundário;\n" \
    "   d) Serviços comerciais cujo valor derive desta funcionalidade.\n" \
    "   O Software é e deve permanecer gratuito para os usuários finais.\n\n" \
    MSG_LEGAL_TXT "3. LICENCIAMENTO COMERCIAL MEDIANTE CONTRATO:" RESET "\n" \
    "   A integração ou distribuição em contexto comercial — mesmo que o Software\n" \
    "   não seja vendido diretamente — EXIGE autorização prévia formalizada.\n" \
    "   Essa autorização deve ser obtida por meio de um CONTRATO DE LICENCIAMENTO\n" \
    "   COMERCIAL assinado pelo autor original (@inrry\x6f\x66\x66), negociado\n" \
    "   individualmente antes do início de qualquer uso comercial.\n" \
    "   Sem esse contrato:\n" \
    "   a) Nenhum uso em produto pago, freemium ou monetizado é permitido;\n" \
    "   b) Nenhuma integração em plataformas SaaS ou serviços pagos é autorizada;\n" \
    "   c) O mero contato prévio não substitui o contrato formalmente assinado.\n\n" \
    "   GitHub:   " MSG_URL "https://github.com/inrry\x6f\x66\x66" RESET "\n" \
    "   Telegram: " MSG_URL "https://t.me/inrry\x6f\x66\x66" RESET "\n\n" \
    MSG_LEGAL_TXT "4. MODIFICAÇÕES E TRABALHOS DERIVADOS:" RESET "\n" \
    "   Modificações são bem-vindas, desde que:\n" \
    "   a) O trabalho derivado não seja comercializado (conforme item 2),\n" \
    "      salvo mediante contrato conforme item 3;\n" \
    "   b) O trabalho derivado seja distribuído publicamente sob esta mesma licença;\n" \
    "   c) Os créditos ao autor original sejam mantidos conforme o item 1,\n" \
    "      com indicação clara de que o trabalho foi modificado e por quem.\n\n" \
    MSG_LEGAL_TXT "5. DISTRIBUIÇÃO EM MÓDULOS E INTEGRAÇÕES:" RESET "\n" \
    "   O uso em módulos (ex: Magisk, KernelSU) ou integrações é permitido, desde que:\n" \
    "   a) A distribuição final seja gratuita para o usuário;\n" \
    "   b) Esta licença acompanhe o Software em qualquer redistribuição;\n" \
    "   c) Os créditos ao autor original sejam preservados.\n\n" \
    MSG_CMD_DIM \
    "O SOFTWARE É FORNECIDO \"NO ESTADO EM QUE SE ENCONTRA\" (AS IS), SEM GARANTIA\n" \
    "DE QUALQUER NATUREZA, EXPRESSA OU IMPLÍCITA. EM NENHUMA HIPÓTESE O AUTOR SERÁ\n" \
    "RESPONSÁVEL POR QUALQUER DANO DIRETO, INDIRETO, INCIDENTAL, ESPECIAL OU\n" \
    "CONSEQUENTE DECORRENTE DO USO OU DA IMPOSSIBILIDADE DE USO DESTE SOFTWARE,\n" \
    "AINDA QUE ADVERTIDO DA POSSIBILIDADE DE TAIS DANOS." RESET "\n"

#define LICENSE_EN \
    "SOFTWARE LICENSE - " LICENSE_LOGO " (C - VERSION)\n" \
    "-------------------------------------------------------------------------------\n" \
    "Copyright (c) 2026 @inrry\x6f\x66\x66 - Licensed under special conditions " LOGO_NEONX " LICENSE\n\n" \
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n" \
    "of this software and associated documentation files, subject to the following:\n\n" \
    MSG_LEGAL_TXT "1. MANDATORY ATTRIBUTION:" RESET "\n" \
    "   The original author's name (@inrry\x6f\x66\x66) and copyright notices must be\n" \
    "   preserved, visibly and legibly, in:\n" \
    "   a) All source files and headers (.c, .h);\n" \
    "   b) The version output of the compiled binary\n" \
    "      (e.g. " BG_FOSCO " neonx --version " RESET ");\n" \
    "   c) Any public documentation or README of a derivative work.\n" \
    "   Removing or obfuscating the author's name by any means\n" \
    "   constitutes a violation of this license.\n\n" \
    MSG_LEGAL_TXT "2. NO COMMERCIALIZATION:" RESET "\n" \
    "   Any form of commercialization is expressly PROHIBITED, including:\n" \
    "   a) Direct sale of the source code or compiled binary;\n" \
    "   b) Rental, paid licensing, or subscription granting access to the Software;\n" \
    "   c) Inclusion in paid products or bundles, even as a secondary component;\n" \
    "   d) Commercial services whose value is directly derived from this Software.\n" \
    "   The Software is and must remain free of charge to end users.\n\n" \
    MSG_LEGAL_TXT "3. COMMERCIAL LICENSING BY CONTRACT:" RESET "\n" \
    "   Integration or distribution in any commercial context — even if the Software\n" \
    "   is not sold directly — REQUIRES prior formalized authorization.\n" \
    "   Such authorization must be obtained through a COMMERCIAL LICENSING AGREEMENT\n" \
    "   signed by the original author (@inrry\x6f\x66\x66), negotiated individually\n" \
    "   before any commercial use begins.\n" \
    "   Without such an agreement:\n" \
    "   a) No use in any paid, freemium, or monetized product is permitted;\n" \
    "   b) No integration into SaaS platforms or paid services is authorized;\n" \
    "   c) Prior contact or stated intent does not substitute a signed agreement.\n\n" \
    "   GitHub:   " MSG_URL "https://github.com/inrry\x6f\x66\x66" RESET "\n" \
    "   Telegram: " MSG_URL "https://t.me/inrry\x6f\x66\x66" RESET "\n\n" \
    MSG_LEGAL_TXT "4. MODIFICATIONS AND DERIVATIVE WORKS:" RESET "\n" \
    "   Modifications are welcome, provided that:\n" \
    "   a) The derivative work is not commercialized (per section 2),\n" \
    "      except under a contract as per section 3;\n" \
    "   b) The derivative work is publicly distributed under this same license;\n" \
    "   c) Credit to the original author is maintained as per section 1,\n" \
    "      with a clear statement that the work has been modified and by whom.\n\n" \
    MSG_LEGAL_TXT "5. DISTRIBUTION IN MODULES AND INTEGRATIONS:" RESET "\n" \
    "   Use in modules (e.g. Magisk, KernelSU) or integrations is permitted, provided:\n" \
    "   a) The final distribution is free of charge to the user;\n" \
    "   b) This license accompanies the Software in any redistribution;\n" \
    "   c) Credit to the original author is preserved.\n\n" \
    MSG_CMD_DIM \
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n" \
    "IMPLIED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,\n" \
    "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT\n" \
    "LIMITED TO PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,\n" \
    "OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE OF\n" \
    "THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE." RESET "\n\n"

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
    },

    // ---------------- [2] ESPANHOL (ES) ----------------
    {
        MSG_ERRO "Error al abrir el archivo\n" RESET,
        "Creador Original: ",
        "Compilado por: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFICADO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_PT,
        LOGO_NEONX DIM ITALIC " - Embellecedor de Terminal Avanzado" RESET "\n",
        "Uso: " BG_FOSCO " cat archivo | neonx [opciones] " RESET "\n\n"
        "  -m [0-11]          Define el estilo de la animación\n"
        "  -s [valor]         Velocidad de transición " MSG_CMD_DIM "(Predeterminado: 0.2)" RESET "\n"
        "  -f [valor]         Frecuencia de la onda " MSG_CMD_DIM "(Predeterminado: 0.3)" RESET "\n"
        "  -d [valor]         Duración en segundos " MSG_CMD_DIM "(0 = Infinito)" RESET "\n"
        "  -max-lines [val]   Límite máximo de líneas " MSG_CMD_DIM "(Predeterminado: 10.000)" RESET "\n"
        "  -A [grados]        Rota el ángulo del gradiente " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [valor]     Define una seed fija " MSG_CMD_DIM "(Determinística)" RESET "\n"
        "  -S                 Renderiza un cuadro estático " MSG_CMD_DIM "(Sin animación)" RESET "\n"
        "  -c [ancho]         Fuerza un ancho estático para el gradiente\n"
        "  -o [0-1]           Ajusta la opacidad horizontal/suavidad\n"
        "  -O [0-1]           Activa la Opacidad Vertical (fading sup/inf)\n"
        "  -F [valor]         Bloquea la tasa de cuadros " MSG_CMD_DIM "(ej: 60, 90)" RESET "\n"
        "  -L                 Procesamiento línea por línea " MSG_CMD_DIM "(Stream)" RESET "\n"
        "  --fo [0-1]         Activa el Modo Mate (Reduce intensidad)\n"
        "  --preset [nombre]  Carga paletas " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias para --color1 y --color2\n"
        "  --color1 [hex]     Define el color inicial del gradiente (ej: #FF0000)\n"
        "  --color2 [hex]     Define el color final del gradiente (ej: #FFA500)\n"
        "  --quantized        Cuantización de colores " MSG_CMD_DIM "(Mayor rendimiento)" RESET "\n"
        "  --no-ansi          Desactiva el uso de colores ANSI en la salida\n"
        "  --spin             Genera códigos ANSI puros " MSG_CMD_DIM "(Para scripts)" RESET "\n"
        "  --lang [id]        Sobrescribe el idioma " MSG_CMD_DIM "(ej: es, en)" RESET "\n"
        "  --license          Muestra los términos de licencia\n"
        "  -v, --version      Muestra la versión y estado del binario\n"
        "  -h, --help         Muestra este panel de ayuda interactivo\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " La opción '%s' requiere un valor numérico.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " La opción '%s' requiere un numérico. Recibido: '%s'\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " El modo de animación (-m) debe ser un entero.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " El modo de animación (-m) debe estar entre 0 y 11.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Opción inválida o argumento suelto '%s'\n",
        "\n" MSG_ERRO "[ ❌ ERROR 404 ]" RESET " ¡No se pasaron datos al binario!\n",
        "\n" MSG_ERRO "[ ❌ ERROR 413 ]" RESET " Archivo muy grande. Usa -L para el modo stream.\n",
        MSG_AVISO "[ ⚠️ AVISO 403 ]" RESET " No se pudo verificar integridad (sistema restringido).\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Fallo al abrir el ejecutable para verificar integridad.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Error de lectura durante la verificación de integridad.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Memoria insuficiente para verificar la integridad.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Firma inválida o formato hexadecimal incorrecto.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " El archivo es muy pequeño para contener la firma.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " La opción '%s' requiere un número entero.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " La opción '%s' requiere un valor positivo.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " La duración no puede ser negativa.\n",
        MSG_ERRO "[ ❌ ERROR 206 ]" RESET " No se pudo asignar memoria (wcsdup).\n",
        MSG_AVISO "[ ⚠️ AVISO 403 ]" RESET " Búfer truncado a 32MB. El renderizado puede corromperse.\n",
        MSG_AVISO "[ ⚠️ AVISO 203 ]" RESET " Fallo al cargar clave personalizada, usando la integrada.\n",
        MSG_CMD_DIM "[ ⚙️ DEPURACIÓN 214 ]" RESET " Desalineamiento geométrico detectado. La estabilización puede variar.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [3] FRANCÊS (FR) ----------------
    {
        MSG_ERRO "Erreur d'ouverture du fichier\n" RESET,
        "Créateur Original : ",
        "Compilé par : ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFIÉ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Embellisseur de Terminal Avancé" RESET "\n",
        "Utilisation: " BG_FOSCO " cat fichier | neonx [options] " RESET "\n\n"
        "  -m [0-11]          Définit le style de l'animation\n"
        "  -s [valeur]        Vitesse de transition " MSG_CMD_DIM "(Défaut: 0.2)" RESET "\n"
        "  -f [valeur]        Fréquence de l'onde " MSG_CMD_DIM "(Défaut: 0.3)" RESET "\n"
        "  -d [valeur]        Durée en secondes " MSG_CMD_DIM "(0 = Infini)" RESET "\n"
        "  -max-lines [val]   Limite maximale de lignes " MSG_CMD_DIM "(Défaut: 10.000)" RESET "\n"
        "  -A [degrés]        Fait pivoter l'angle du dégradé " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [valeur]    Définit une graine fixe " MSG_CMD_DIM "(Déterministe)" RESET "\n"
        "  -S                 Rendu d'une image statique " MSG_CMD_DIM "(Sans animation)" RESET "\n"
        "  -c [largeur]       Force une largeur statique pour le dégradé\n"
        "  -o [0-1]           Ajuste l'opacité horizontale/lissage\n"
        "  -O [0-1]           Active l'Opacité Verticale (fondu haut/bas)\n"
        "  -F [valeur]        Verrouille la fréquence d'images " MSG_CMD_DIM "(ex: 60, 90)" RESET "\n"
        "  -L                 Traitement ligne par ligne " MSG_CMD_DIM "(Flux)" RESET "\n"
        "  --fo [0-1]         Active le Mode Mat (Réduit la vivacité)\n"
        "  --preset [nom]     Charge des palettes " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias pour --color1 et --color2\n"
        "  --color1 [hex]     Définit la couleur de début du dégradé (ex: #FF0000)\n"
        "  --color2 [hex]     Définit la couleur de fin du dégradé (ex: #FFA500)\n"
        "  --quantized        Quantification des couleurs " MSG_CMD_DIM "(Meilleures performances)" RESET "\n"
        "  --no-ansi          Désactive les couleurs ANSI en sortie\n"
        "  --spin             Génère des codes ANSI purs " MSG_CMD_DIM "(Pour les scripts)" RESET "\n"
        "  --lang [id]        Écrase la langue " MSG_CMD_DIM "(ex: fr, en)" RESET "\n"
        "  --license          Affiche les termes de la licence\n"
        "  -v, --version      Affiche la version et l'état du binaire\n"
        "  -h, --help         Affiche ce panneau d'aide interactif\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " L'option '%s' nécessite une valeur numérique.\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " L'option '%s' nécessite un nombre. Reçu : '%s'\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " Le mode d'animation (-m) doit être un entier.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " Le mode d'animation (-m) doit être entre 0 et 11.\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " Option invalide ou argument isolé '%s'\n",
        "\n" MSG_ERRO "[ ❌ ERREUR 404 ]" RESET " Aucune donnée n'a été transmise au binaire !\n",
        "\n" MSG_ERRO "[ ❌ ERREUR 413 ]" RESET " Fichier trop grand. Utilisez -L pour le mode stream.\n",
        MSG_AVISO "[ ⚠️ AVERT 403 ]" RESET " Impossible de vérifier l'intégrité (système restreint).\n",
        MSG_ERRO "[ ❌ ERREUR 403 ]" RESET " Échec d'ouverture de l'exécutable pour vérification.\n",
        MSG_ERRO "[ ❌ ERREUR 403 ]" RESET " Erreur de lecture lors de la vérification de l'intégrité.\n",
        MSG_ERRO "[ ❌ ERREUR 403 ]" RESET " Mémoire insuffisante pour vérifier l'intégrité.\n",
        MSG_ERRO "[ ❌ ERREUR 403 ]" RESET " Signature invalide ou format hexadécimal incorrect.\n",
        MSG_ERRO "[ ❌ ERREUR 403 ]" RESET " Le fichier est trop petit pour contenir la signature.\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " L'option '%s' nécessite un entier.\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " L'option '%s' nécessite une valeur positive.\n",
        MSG_ERRO "[ ❌ ERREUR 400 ]" RESET " La durée ne peut pas être négative.\n",
        MSG_ERRO "[ ❌ ERREUR 206 ]" RESET " Impossible d'allouer la mémoire (wcsdup).\n",
        MSG_AVISO "[ ⚠️ AVERT 403 ]" RESET " Tampon tronqué à 32Mo. Le rendu peut être corrompu.\n",
        MSG_AVISO "[ ⚠️ AVERT 203 ]" RESET " Échec du chargement de la clé, utilisation de celle par défaut.\n",
        MSG_CMD_DIM "[ ⚙️ DÉBOGAGE 214 ]" RESET " Désalignement géométrique détecté. La stabilisation peut varier.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [4] ALEMÃO (DE) ----------------
    {
        MSG_ERRO "Fehler beim Öffnen der Datei\n" RESET,
        "Originalentwickler: ",
        "Kompiliert von: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFIZIERT" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Erweitertes Terminal-Verschönerungstool" RESET "\n",
        "Verwendung: " BG_FOSCO " cat datei | neonx [optionen] " RESET "\n\n"
        "  -m [0-11]          Animationsstil festlegen\n"
        "  -s [wert]          Übergangsgeschwindigkeit " MSG_CMD_DIM "(Standard: 0.2)" RESET "\n"
        "  -f [wert]          Wellenfrequenz " MSG_CMD_DIM "(Standard: 0.3)" RESET "\n"
        "  -d [wert]          Dauer in Sekunden " MSG_CMD_DIM "(0 = Unendlich)" RESET "\n"
        "  -max-lines [wert]  Maximale Zeilenanzahl " MSG_CMD_DIM "(Standard: 10.000)" RESET "\n"
        "  -A [grad]          Gradientenwinkel drehen " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [wert]      Festen Seed definieren " MSG_CMD_DIM "(Deterministisch)" RESET "\n"
        "  -S                 Statischen Frame rendern " MSG_CMD_DIM "(Ohne Animation)" RESET "\n"
        "  -c [breite]        Statische Breite für Gradienten erzwingen\n"
        "  -o [0-1]           Horizontale Deckkraft/Glättung anpassen\n"
        "  -O [0-1]           Vertikale Deckkraft aktivieren (Fading oben/unten)\n"
        "  -F [wert]          Bildrate sperren " MSG_CMD_DIM "(z.B.: 60, 90)" RESET "\n"
        "  -L                 Zeilenweise Verarbeitung " MSG_CMD_DIM "(Stream)" RESET "\n"
        "  --fo [0-1]         Matt-Modus aktivieren (Reduziert Lebendigkeit)\n"
        "  --preset [name]    Paletten laden " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias für --color1 und --color2\n"
        "  --color1 [hex]     Startfarbe des Gradienten festlegen (z.B.: #FF0000)\n"
        "  --color2 [hex]     Endfarbe des Gradienten festlegen (z.B.: #FFA500)\n"
        "  --quantized        Farbquantisierung " MSG_CMD_DIM "(Höhere Leistung)" RESET "\n"
        "  --no-ansi          ANSI-Farben in der Ausgabe deaktivieren\n"
        "  --spin             Reine ANSI-Codes generieren " MSG_CMD_DIM "(Für Skripte)" RESET "\n"
        "  --lang [id]        Sprache überschreiben " MSG_CMD_DIM "(z.B.: de, en)" RESET "\n"
        "  --license          Lizenzbedingungen anzeigen\n"
        "  -v, --version      Version und Binärstatus anzeigen\n"
        "  -h, --help         Dieses interaktive Hilfefenster anzeigen\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Die Option '%s' erfordert einen numerischen Wert.\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Option '%s' erfordert eine Zahl. Erhalten: '%s'\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Der Animationsmodus (-m) muss eine Ganzzahl sein.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " Der Animationsmodus (-m) muss zwischen 0 und 11 liegen.\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Ungültige Option oder loses Argument '%s'\n",
        "\n" MSG_ERRO "[ ❌ FEHLER 404 ]" RESET " Es wurden keine Daten an das Binary übergeben!\n",
        "\n" MSG_ERRO "[ ❌ FEHLER 413 ]" RESET " Datei zu groß. Verwende -L für den Stream-Modus.\n",
        MSG_AVISO "[ ⚠️ WARNUNG 403 ]" RESET " Integrität konnte nicht geprüft werden (eingeschränktes System).\n",
        MSG_ERRO "[ ❌ FEHLER 403 ]" RESET " Ausführbare Datei konnte nicht zur Prüfung geöffnet werden.\n",
        MSG_ERRO "[ ❌ FEHLER 403 ]" RESET " Lesefehler bei der Integritätsprüfung.\n",
        MSG_ERRO "[ ❌ FEHLER 403 ]" RESET " Unzureichender Speicher für die Integritätsprüfung.\n",
        MSG_ERRO "[ ❌ FEHLER 403 ]" RESET " Ungültige Signatur oder falsches Hex-Format.\n",
        MSG_ERRO "[ ❌ FEHLER 403 ]" RESET " Die Datei ist zu klein, um die Signatur zu enthalten.\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Die Option '%s' erfordert eine Ganzzahl.\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Die Option '%s' erfordert einen positiven Wert.\n",
        MSG_ERRO "[ ❌ FEHLER 400 ]" RESET " Die Dauer darf nicht negativ sein.\n",
        MSG_ERRO "[ ❌ FEHLER 206 ]" RESET " Speicher konnte nicht zugewiesen werden (wcsdup).\n",
        MSG_AVISO "[ ⚠️ WARNUNG 403 ]" RESET " Puffer auf 32 MB begrenzt. Rendering könnte fehlschlagen.\n",
        MSG_AVISO "[ ⚠️ WARNUNG 203 ]" RESET " Benutzerdefinierter Schlüssel fehlgeschlagen, verwende internen.\n",
        MSG_CMD_DIM "[ ⚙️ DÉBOGAGE 214 ]" RESET " Désalignement géométrique détecté. La stabilisation peut varier.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [5] ITALIANO (IT) ----------------
    {
        MSG_ERRO "Errore durante l'apertura del file\n" RESET,
        "Creatore Originale: ",
        "Compilato da: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFICATO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Abbellitore di Terminale Avanzato" RESET "\n",
        "Uso: " BG_FOSCO " cat file | neonx [opzioni] " RESET "\n\n"
        "  -m [0-11]          Imposta lo stile dell'animazione\n"
        "  -s [valore]        Velocità di transizione " MSG_CMD_DIM "(Predefinito: 0.2)" RESET "\n"
        "  -f [valore]        Frequenza dell'onda " MSG_CMD_DIM "(Predefinito: 0.3)" RESET "\n"
        "  -d [valore]        Durata in secondi " MSG_CMD_DIM "(0 = Infinito)" RESET "\n"
        "  -max-lines [val]   Limite massimo di righe " MSG_CMD_DIM "(Predefinito: 10.000)" RESET "\n"
        "  -A [gradi]         Ruota l'angolo del gradiente " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [valore]    Imposta un seed fisso " MSG_CMD_DIM "(Deterministico)" RESET "\n"
        "  -S                 Renderizza un frame statico " MSG_CMD_DIM "(Senza animazione)" RESET "\n"
        "  -c [larg]          Forza una larghezza statica per il gradiente\n"
        "  -o [0-1]           Regola l'opacità orizzontale/morbidezza\n"
        "  -O [0-1]           Attiva Opacità Verticale (dissolvenza alto/basso)\n"
        "  -F [valore]        Blocca il framerate " MSG_CMD_DIM "(es: 60, 90)" RESET "\n"
        "  -L                 Elaborazione riga per riga " MSG_CMD_DIM "(Stream)" RESET "\n"
        "  --fo [0-1]         Attiva Modalità Opaca (Riduce la vividezza)\n"
        "  --preset [nome]    Carica palette " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias per --color1 e --color2\n"
        "  --color1 [hex]     Imposta il colore iniziale del gradiente (es: #FF0000)\n"
        "  --color2 [hex]     Imposta il colore finale del gradiente (es: #FFA500)\n"
        "  --quantized        Quantizzazione dei colori " MSG_CMD_DIM "(Prestazioni migliori)" RESET "\n"
        "  --no-ansi          Disattiva i colori ANSI nell'output\n"
        "  --spin             Genera codici ANSI puri " MSG_CMD_DIM "(Per script)" RESET "\n"
        "  --lang [id]        Sovrascrive la lingua " MSG_CMD_DIM "(es: it, en)" RESET "\n"
        "  --license          Mostra i termini di licenza\n"
        "  -v, --version      Mostra la versione e lo stato del binario\n"
        "  -h, --help         Mostra questo pannello di aiuto interattivo\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " L'opzione '%s' richiede un valore numerico.\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " L'opzione '%s' richiede un numero. Ricevuto: '%s'\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " La modalità di animazione (-m) deve essere un intero.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " La modalità di animazione (-m) deve essere compresa tra 0 e 11.\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " Opzione non valida o argomento sciolto '%s'\n",
        "\n" MSG_ERRO "[ ❌ ERRORE 404 ]" RESET " Nessun dato passato al binario!\n",
        "\n" MSG_ERRO "[ ❌ ERRORE 413 ]" RESET " File troppo grande. Usa -L per la modalità stream.\n",
        MSG_AVISO "[ ⚠️ AVVISO 403 ]" RESET " Impossibile verificare l'integrità (sistema limitato).\n",
        MSG_ERRO "[ ❌ ERRORE 403 ]" RESET " Impossibile aprire l'eseguibile per la verifica dell'integrità.\n",
        MSG_ERRO "[ ❌ ERRORE 403 ]" RESET " Errore di lettura durante la verifica dell'integrità.\n",
        MSG_ERRO "[ ❌ ERRORE 403 ]" RESET " Memoria insufficiente per verificare l'integrità.\n",
        MSG_ERRO "[ ❌ ERRORE 403 ]" RESET " Firma non valida o formato esadecimale errato.\n",
        MSG_ERRO "[ ❌ ERRORE 403 ]" RESET " Il file è troppo piccolo per contenere la firma.\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " L'opzione '%s' richiede un numero intero.\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " L'opzione '%s' richiede un valore positivo.\n",
        MSG_ERRO "[ ❌ ERRORE 400 ]" RESET " La durata non può essere negativa.\n",
        MSG_ERRO "[ ❌ ERRORE 206 ]" RESET " Impossibile allocare memoria (wcsdup).\n",
        MSG_AVISO "[ ⚠️ AVVISO 403 ]" RESET " Buffer troncato a 32MB. Il rendering potrebbe corrompersi.\n",
        MSG_AVISO "[ ⚠️ AVVISO 203 ]" RESET " Impossibile caricare la chiave, utilizzo quella integrata.\n",
        MSG_CMD_DIM "[ ⚙️ DEBUG 214 ]" RESET " Disallineamento geometrico rilevato. La stabilizzazione può variare.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [6] RUSSO (RU) ----------------
    {
        MSG_ERRO "Ошибка при открытии файла\n" RESET,
        "Оригинальный создатель: ",
        "Скомпилировано: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ИЗМЕНЕНО" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Продвинутый Улучшитель Терминала" RESET "\n",
        "Использование: " BG_FOSCO " cat файл | neonx [опции] " RESET "\n\n"
        "  -m [0-11]          Задает стиль анимации\n"
        "  -s [значение]      Скорость перехода " MSG_CMD_DIM "(По умолчанию: 0.2)" RESET "\n"
        "  -f [значение]      Частота волны " MSG_CMD_DIM "(По умолчанию: 0.3)" RESET "\n"
        "  -d [значение]      Продолжительность в секундах " MSG_CMD_DIM "(0 = Бесконечно)" RESET "\n"
        "  -max-lines [знач]  Максимальный лимит строк " MSG_CMD_DIM "(По умолчанию: 10.000)" RESET "\n"
        "  -A [градусы]       Поворачивает угол градиента " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [значение]  Задает фиксированный seed " MSG_CMD_DIM "(Детерминировано)" RESET "\n"
        "  -S                 Рендерит статический кадр " MSG_CMD_DIM "(Без анимации)" RESET "\n"
        "  -c [ширина]        Принудительная статическая ширина для градиента\n"
        "  -o [0-1]           Настраивает горизонтальную непрозрачность/сглаживание\n"
        "  -O [0-1]           Включает вертикальную непрозрачность (затухание верх/низ)\n"
        "  -F [значение]      Блокирует частоту кадров " MSG_CMD_DIM "(напр.: 60, 90)" RESET "\n"
        "  -L                 Построчная обработка " MSG_CMD_DIM "(Поток)" RESET "\n"
        "  --fo [0-1]         Включает матовый режим (Снижает яркость)\n"
        "  --preset [имя]     Загружает палитры " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Псевдонимы для --color1 и --color2\n"
        "  --color1 [hex]     Начальный цвет градиента (напр.: #FF0000)\n"
        "  --color2 [hex]     Конечный цвет градиента (напр.: #FFA500)\n"
        "  --quantized        Квантование цветов " MSG_CMD_DIM "(Высокая производительность)" RESET "\n"
        "  --no-ansi          Отключает цвета ANSI в выводе\n"
        "  --spin             Генерирует чистые коды ANSI " MSG_CMD_DIM "(Для скриптов)" RESET "\n"
        "  --lang [id]        Переопределяет язык " MSG_CMD_DIM "(напр.: ru, en)" RESET "\n"
        "  --license          Показывает условия лицензии\n"
        "  -v, --version      Показывает версию и статус бинарника\n"
        "  -h, --help         Показывает эту интерактивную панель помощи\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Опция '%s' требует числового значения.\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Опция '%s' требует числа. Получено: '%s'\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Режим анимации (-m) должен быть целым числом.\n",
        MSG_INFO "[ ℹ️ ИНФО 416 ]" RESET " Режим анимации (-m) должен быть от 0 до 11.\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Неверная опция или аргумент '%s'\n",
        "\n" MSG_ERRO "[ ❌ ОШИБКА 404 ]" RESET " Данные не переданы в бинарник!\n",
        "\n" MSG_ERRO "[ ❌ ОШИБКА 413 ]" RESET " Файл слишком большой. Используйте -L для stream.\n",
        MSG_AVISO "[ ⚠️ ПРЕДУПР 403 ]" RESET " Не удалось проверить целостность (система ограничена).\n",
        MSG_ERRO "[ ❌ ОШИБКА 403 ]" RESET " Ошибка открытия исполняемого файла для проверки целостности.\n",
        MSG_ERRO "[ ❌ ОШИБКА 403 ]" RESET " Ошибка чтения при проверке целостности.\n",
        MSG_ERRO "[ ❌ ОШИБКА 403 ]" RESET " Недостаточно памяти для проверки целостности.\n",
        MSG_ERRO "[ ❌ ОШИБКА 403 ]" RESET " Неверная подпись или неправильный hex-формат.\n",
        MSG_ERRO "[ ❌ ОШИБКА 403 ]" RESET " Файл слишком мал для хранения подписи.\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Опция '%s' требует целого числа.\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Опция '%s' требует положительного значения.\n",
        MSG_ERRO "[ ❌ ОШИБКА 400 ]" RESET " Продолжительность не может быть отрицательной.\n",
        MSG_ERRO "[ ❌ ОШИБКА 206 ]" RESET " Не удалось выделить память (wcsdup).\n",
        MSG_AVISO "[ ⚠️ ПРЕДУПР 403 ]" RESET " Буфер усечен до 32 МБ. Рендеринг может быть поврежден.\n",
        MSG_AVISO "[ ⚠️ ПРЕДУПР 203 ]" RESET " Ошибка загрузки ключа, используется встроенный.\n",
        MSG_CMD_DIM "[ ⚙️ ОТЛАДКА 214 ]" RESET " Обнаружено геометрическое смещение. Стабилизация может варьироваться.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [7] CHINÊS (ZH) ----------------
    {
        MSG_ERRO "打开文件时出错\n" RESET,
        "原作者: ",
        "编译者: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "已修改" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - 高级终端美化工具" RESET "\n",
        "用法: " BG_FOSCO " cat 文件 | neonx [选项] " RESET "\n\n"
        "  -m [0-11]          设置动画风格\n"
        "  -s [值]            过渡速度 " MSG_CMD_DIM "(默认: 0.2)" RESET "\n"
        "  -f [值]            波浪频率 " MSG_CMD_DIM "(默认: 0.3)" RESET "\n"
        "  -d [值]            持续时间（秒） " MSG_CMD_DIM "(0 = 无限)" RESET "\n"
        "  -max-lines [值]    最大行数限制 " MSG_CMD_DIM "(默认: 10.000)" RESET "\n"
        "  -A [度]            旋转渐变角度 " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [值]        设置固定种子 " MSG_CMD_DIM "(确定性)" RESET "\n"
        "  -S                 渲染静态帧 " MSG_CMD_DIM "(无动画)" RESET "\n"
        "  -c [宽度]          强制渐变的静态宽度\n"
        "  -o [0-1]           调整水平不透明度/平滑度\n"
        "  -O [0-1]           启用垂直不透明度 (顶部/底部淡入淡出)\n"
        "  -F [值]            锁定帧率 " MSG_CMD_DIM "(例: 60, 90)" RESET "\n"
        "  -L                 逐行处理 " MSG_CMD_DIM "(流)" RESET "\n"
        "  --fo [0-1]         启用哑光模式 (降低鲜艳度)\n"
        "  --preset [名称]    加载调色板 " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   --color1 和 --color2 的别名\n"
        "  --color1 [hex]     设置渐变起始颜色 (例: #FF0000)\n"
        "  --color2 [hex]     设置渐变结束颜色 (例: #FFA500)\n"
        "  --quantized        颜色量化 " MSG_CMD_DIM "(更高性能)" RESET "\n"
        "  --no-ansi          在输出中禁用 ANSI 颜色\n"
        "  --spin             生成纯 ANSI 代码 " MSG_CMD_DIM "(用于脚本)" RESET "\n"
        "  --lang [id]        覆盖语言 " MSG_CMD_DIM "(例: zh, en)" RESET "\n"
        "  --license          显示许可条款\n"
        "  -v, --version      显示二进制版本和状态\n"
        "  -h, --help         显示此交互式帮助面板\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 选项 '%s' 后需要一个数值。\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 选项 '%s' 需要一个数值。收到: '%s'\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 动画模式 (-m) 必须是一个整数。\n",
        MSG_INFO "[ ℹ️ 信息 416 ]" RESET " 动画模式 (-m) 必须在 0 到 11 之间。\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 无效选项或独立参数 '%s'\n",
        "\n" MSG_ERRO "[ ❌ 错误 404 ]" RESET " 没有向二进制传递任何数据！\n",
        "\n" MSG_ERRO "[ ❌ 错误 413 ]" RESET " 文件太大。请使用 -L 进行流模式。\n",
        MSG_AVISO "[ ⚠️ 警告 403 ]" RESET " 无法验证完整性（受限系统或被篡改的二进制）。\n",
        MSG_ERRO "[ ❌ 错误 403 ]" RESET " 无法打开可执行文件本身以进行完整性验证。\n",
        MSG_ERRO "[ ❌ 错误 403 ]" RESET " 完整性验证期间文件读取错误。\n",
        MSG_ERRO "[ ❌ 错误 403 ]" RESET " 内存不足，无法验证完整性。\n",
        MSG_ERRO "[ ❌ 错误 403 ]" RESET " 签名无效或十六进制格式不正确。\n",
        MSG_ERRO "[ ❌ 错误 403 ]" RESET " 文件太小，无法包含完整性签名。\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 选项 '%s' 需要一个整数。\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 选项 '%s' 需要一个正值。\n",
        MSG_ERRO "[ ❌ 错误 400 ]" RESET " 持续时间不能为负数。\n",
        MSG_ERRO "[ ❌ 错误 206 ]" RESET " 无法分配内存 (wcsdup)。\n",
        MSG_AVISO "[ ⚠️ 警告 403 ]" RESET " 缓冲区已被预防性截断至 32MB。渲染可能会损坏。\n",
        MSG_AVISO "[ ⚠️ 警告 203 ]" RESET " 加载自定义密钥失败，使用内置密钥。\n",
        MSG_CMD_DIM "[ ⚙️ 调试 214 ]" RESET " 检测到几何错位。稳定效果可能有所不同。\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [8] JAPONÊS (JA) ----------------
    {
        MSG_ERRO "ファイルを開く際にエラーが発生しました\n" RESET,
        "オリジナルの作成者: ",
        "コンパイル: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "変更済み" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - 高度なターミナル装飾ツール" RESET "\n",
        "使用法: " BG_FOSCO " cat ファイル | neonx [オプション] " RESET "\n\n"
        "  -m [0-11]          アニメーションスタイルを設定\n"
        "  -s [値]            トランジション速度 " MSG_CMD_DIM "(デフォルト: 0.2)" RESET "\n"
        "  -f [値]            波の周波数 " MSG_CMD_DIM "(デフォルト: 0.3)" RESET "\n"
        "  -d [値]            継続時間（秒） " MSG_CMD_DIM "(0 = 無限)" RESET "\n"
        "  -max-lines [値]    最大行数制限 " MSG_CMD_DIM "(デフォルト: 10.000)" RESET "\n"
        "  -A [度]            グラデーションの角度を回転 " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [値]        固定シードを設定 " MSG_CMD_DIM "(決定的)" RESET "\n"
        "  -S                 静的フレームをレンダリング " MSG_CMD_DIM "(アニメーションなし)" RESET "\n"
        "  -c [幅]            グラデーションの静的な幅を強制\n"
        "  -o [0-1]           水平方向の不透明度/滑らかさを調整\n"
        "  -O [0-1]           垂直方向の不透明度を有効化 (上下のフェード)\n"
        "  -F [値]            フレームレートを固定 " MSG_CMD_DIM "(例: 60, 90)" RESET "\n"
        "  -L                 行単位の処理 " MSG_CMD_DIM "(ストリーム)" RESET "\n"
        "  --fo [0-1]         マットモードを有効化 (鮮やかさを軽減)\n"
        "  --preset [名前]    パレットを読み込む " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   --color1 および --color2 のエイリアス\n"
        "  --color1 [hex]     グラデーションの開始色を設定 (例: #FF0000)\n"
        "  --color2 [hex]     グラデーションの終了色を設定 (例: #FFA500)\n"
        "  --quantized        色の量子化 " MSG_CMD_DIM "(高パフォーマンス)" RESET "\n"
        "  --no-ansi          出力でのANSIカラーを無効化\n"
        "  --spin             純粋なANSIコードを生成 " MSG_CMD_DIM "(スクリプト用)" RESET "\n"
        "  --lang [id]        言語を上書き " MSG_CMD_DIM "(例: ja, en)" RESET "\n"
        "  --license          ライセンス条項を表示\n"
        "  -v, --version      バイナリのバージョンとステータスを表示\n"
        "  -h, --help         このインタラクティブなヘルプパネルを表示\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " オプション '%s' には数値が必要です。\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " オプション '%s' には数値が必要です。受信: '%s'\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " アニメーションモード (-m) は整数である必要があります。\n",
        MSG_INFO "[ ℹ️ 情報 416 ]" RESET " アニメーションモード (-m) は0から11の間である必要があります。\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " 無効なオプションまたは独立した引数 '%s'\n",
        "\n" MSG_ERRO "[ ❌ エラー 404 ]" RESET " バイナリにデータが渡されていません！\n",
        "\n" MSG_ERRO "[ ❌ エラー 413 ]" RESET " ファイルが大きすぎます。ストリームモードには -L を使用してください。\n",
        MSG_AVISO "[ ⚠️ 警告 403 ]" RESET " 整合性を確認できませんでした（制限されたシステム）。\n",
        MSG_ERRO "[ ❌ エラー 403 ]" RESET " 整合性確認のために実行可能ファイルを開くのに失敗しました。\n",
        MSG_ERRO "[ ❌ エラー 403 ]" RESET " 整合性確認中のファイル読み取りエラー。\n",
        MSG_ERRO "[ ❌ エラー 403 ]" RESET " 整合性を確認するためのメモリが不足しています。\n",
        MSG_ERRO "[ ❌ エラー 403 ]" RESET " 無効な署名または不正な16進数フォーマット。\n",
        MSG_ERRO "[ ❌ エラー 403 ]" RESET " ファイルが小さすぎて署名が含まれていません。\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " オプション '%s' には整数が必要です。\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " オプション '%s' には正の値が必要です。\n",
        MSG_ERRO "[ ❌ エラー 400 ]" RESET " 期間は負にすることはできません。\n",
        MSG_ERRO "[ ❌ エラー 206 ]" RESET " メモリを割り当てることができませんでした (wcsdup)。\n",
        MSG_AVISO "[ ⚠️ 警告 403 ]" RESET " バッファは32MBに切り捨てられました。レンダリングが破損する可能性があります。\n",
        MSG_AVISO "[ ⚠️ 警告 203 ]" RESET " カスタムキーの読み込みに失敗しました。組み込みキーを使用します。\n",
        MSG_CMD_DIM "[ ⚙️ デバッグ 214 ]" RESET " 幾何学的なズレを検出しました。安定化処理は変動する可能性があります。\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [9] COREANO (KO) ----------------
    {
        MSG_ERRO "파일 열기 오류\n" RESET,
        "원본 제작자: ",
        "컴파일러: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "수정됨" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - 고급 터미널 꾸미기 도구" RESET "\n",
        "사용법: " BG_FOSCO " cat 파일 | neonx [옵션] " RESET "\n\n"
        "  -m [0-11]          애니메이션 스타일 설정\n"
        "  -s [값]            전환 속도 " MSG_CMD_DIM "(기본값: 0.2)" RESET "\n"
        "  -f [값]            파동 주파수 " MSG_CMD_DIM "(기본값: 0.3)" RESET "\n"
        "  -d [값]            지속 시간(초) " MSG_CMD_DIM "(0 = 무한)" RESET "\n"
        "  -max-lines [값]    최대 줄 수 제한 " MSG_CMD_DIM "(기본값: 10.000)" RESET "\n"
        "  -A [도]            그라데이션 각도 회전 " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [값]        고정 시드 설정 " MSG_CMD_DIM "(결정론적)" RESET "\n"
        "  -S                 정적 프레임 렌더링 " MSG_CMD_DIM "(애니메이션 없음)" RESET "\n"
        "  -c [너비]          그라데이션의 정적 너비 강제 적용\n"
        "  -o [0-1]           가로 불투명도/부드러움 조정\n"
        "  -O [0-1]           세로 불투명도 활성화 (상단/하단 페이딩)\n"
        "  -F [값]            프레임 속도 고정 " MSG_CMD_DIM "(예: 60, 90)" RESET "\n"
        "  -L                 줄 단위 처리 " MSG_CMD_DIM "(스트림)" RESET "\n"
        "  --fo [0-1]         매트 모드 활성화 (선명도 감소)\n"
        "  --preset [이름]    팔레트 로드 " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   --color1 및 --color2의 별칭\n"
        "  --color1 [hex]     그라데이션 시작 색상 설정 (예: #FF0000)\n"
        "  --color2 [hex]     그라데이션 끝 색상 설정 (예: #FFA500)\n"
        "  --quantized        색상 양자화 " MSG_CMD_DIM "(더 높은 성능)" RESET "\n"
        "  --no-ansi          출력에서 ANSI 색상 비활성화\n"
        "  --spin             순수 ANSI 코드 생성 " MSG_CMD_DIM "(스크립트용)" RESET "\n"
        "  --lang [id]        언어 재정의 " MSG_CMD_DIM "(예: ko, en)" RESET "\n"
        "  --license          라이선스 조건 표시\n"
        "  -v, --version      바이너리 버전 및 상태 표시\n"
        "  -h, --help         이 대화형 도움말 패널 표시\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " '%s' 옵션 뒤에 숫자 값이 필요합니다.\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " '%s' 옵션에 숫자가 필요합니다. 수신됨: '%s'\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " 애니메이션 모드(-m)는 정수여야 합니다.\n",
        MSG_INFO "[ ℹ️ 정보 416 ]" RESET " 애니메이션 모드(-m)는 0에서 11 사이여야 합니다.\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " 잘못된 옵션 또는 독립된 인수 '%s'\n",
        "\n" MSG_ERRO "[ ❌ 오류 404 ]" RESET " 바이너리에 전달된 데이터가 없습니다!\n",
        "\n" MSG_ERRO "[ ❌ 오류 413 ]" RESET " 파일이 너무 큽니다. 스트림 모드에는 -L을 사용하세요.\n",
        MSG_AVISO "[ ⚠️ 경고 403 ]" RESET " 무결성을 확인할 수 없습니다 (제한된 시스템).\n",
        MSG_ERRO "[ ❌ 오류 403 ]" RESET " 무결성 검증을 위해 실행 파일을 여는 데 실패했습니다.\n",
        MSG_ERRO "[ ❌ 오류 403 ]" RESET " 무결성 검증 중 파일 읽기 오류.\n",
        MSG_ERRO "[ ❌ 오류 403 ]" RESET " 무결성을 확인할 메모리가 부족합니다.\n",
        MSG_ERRO "[ ❌ 오류 403 ]" RESET " 잘못된 서명 또는 잘못된 16진수 형식.\n",
        MSG_ERRO "[ ❌ 오류 403 ]" RESET " 파일이 서명을 포함하기에 너무 작습니다.\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " '%s' 옵션에는 정수가 필요합니다.\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " '%s' 옵션에는 양수 값이 필요합니다.\n",
        MSG_ERRO "[ ❌ 오류 400 ]" RESET " 기간은 음수일 수 없습니다.\n",
        MSG_ERRO "[ ❌ 오류 206 ]" RESET " 메모리를 할당할 수 없습니다 (wcsdup).\n",
        MSG_AVISO "[ ⚠️ 경고 403 ]" RESET " 버퍼가 32MB로 잘렸습니다. 렌더링이 손상될 수 있습니다.\n",
        MSG_AVISO "[ ⚠️ 경고 203 ]" RESET " 사용자 지정 키를 로드하지 못했습니다. 기본 키를 사용합니다.\n",
        MSG_CMD_DIM "[ ⚙️ 디버그 214 ]" RESET " 기하학적 불일치가 감지되었습니다. 안정화는 달라질 수 있습니다.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [10] TURCO (TR) ----------------
    {
        MSG_ERRO "Dosya açılırken hata oluştu\n" RESET,
        "Orijinal Yapımcı: ",
        "Derleyen: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "DEĞİŞTİRİLDİ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Gelişmiş Terminal Güzelleştirici" RESET "\n",
        "Kullanım: " BG_FOSCO " cat dosya | neonx [seçenekler] " RESET "\n\n"
        "  -m [0-11]          Animasyon stilini belirler\n"
        "  -s [değer]         Geçiş hızı " MSG_CMD_DIM "(Varsayılan: 0.2)" RESET "\n"
        "  -f [değer]         Dalga frekansı " MSG_CMD_DIM "(Varsayılan: 0.3)" RESET "\n"
        "  -d [değer]         Saniye cinsinden süre " MSG_CMD_DIM "(0 = Sonsuz)" RESET "\n"
        "  -max-lines [değ]   Maksimum satır sınırı " MSG_CMD_DIM "(Varsayılan: 10.000)" RESET "\n"
        "  -A [derece]        Gradyan açısını döndürür " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [değer]     Sabit bir seed ayarlar " MSG_CMD_DIM "(Deterministik)" RESET "\n"
        "  -S                 Statik bir kare oluşturur " MSG_CMD_DIM "(Animasyon yok)" RESET "\n"
        "  -c [genişlik]      Gradyan için statik bir genişlik zorlar\n"
        "  -o [0-1]           Yatay opaklığı/pürüzsüzlüğü ayarlar\n"
        "  -O [0-1]           Dikey Opaklığı etkinleştirir (üst/alt solma)\n"
        "  -F [değer]         Kare hızını kilitler " MSG_CMD_DIM "(örn: 60, 90)" RESET "\n"
        "  -L                 Satır satır işleme " MSG_CMD_DIM "(Akış)" RESET "\n"
        "  --fo [0-1]         Mat Modu etkinleştirir (Canlılığı azaltır)\n"
        "  --preset [isim]    Paletleri yükler " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   --color1 ve --color2 için kısayol\n"
        "  --color1 [hex]     Gradyanın başlangıç rengini belirler (örn: #FF0000)\n"
        "  --color2 [hex]     Gradyanın bitiş rengini belirler (örn: #FFA500)\n"
        "  --quantized        Renk niceleme " MSG_CMD_DIM "(Daha yüksek performans)" RESET "\n"
        "  --no-ansi          Çıktıda ANSI renklerini devre dışı bırakır\n"
        "  --spin             Saf ANSI kodları üretir " MSG_CMD_DIM "(Betikler için)" RESET "\n"
        "  --lang [id]        Dili geçersiz kılar " MSG_CMD_DIM "(örn: tr, en)" RESET "\n"
        "  --license          Lisans koşullarını gösterir\n"
        "  -v, --version      Sürümü ve ikili dosya durumunu gösterir\n"
        "  -h, --help         Bu etkileşimli yardım panelini gösterir\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " '%s' seçeneği sayısal bir değer gerektirir.\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " '%s' seçeneği sayı gerektirir. Alınan: '%s'\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " Animasyon modu (-m) tam sayı olmalıdır.\n",
        MSG_INFO "[ ℹ️ BİLGİ 416 ]" RESET " Animasyon modu (-m) 0 ile 11 arasında olmalıdır.\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " Geçersiz seçenek veya boşta argüman '%s'\n",
        "\n" MSG_ERRO "[ ❌ HATA 404 ]" RESET " İkili dosyaya hiçbir veri geçilmedi!\n",
        "\n" MSG_ERRO "[ ❌ HATA 413 ]" RESET " Dosya çok büyük. Akış modu için -L kullanın.\n",
        MSG_AVISO "[ ⚠️ UYARI 403 ]" RESET " Bütünlük doğrulanamadı (kısıtlı sistem).\n",
        MSG_ERRO "[ ❌ HATA 403 ]" RESET " Doğrulama için yürütülebilir dosya açılamadı.\n",
        MSG_ERRO "[ ❌ HATA 403 ]" RESET " Bütünlük doğrulaması sırasında dosya okuma hatası.\n",
        MSG_ERRO "[ ❌ HATA 403 ]" RESET " Bütünlüğü doğrulamak için yetersiz bellek.\n",
        MSG_ERRO "[ ❌ HATA 403 ]" RESET " Geçersiz imza veya yanlış onaltılık biçim.\n",
        MSG_ERRO "[ ❌ HATA 403 ]" RESET " Dosya, imzayı barındırmak için çok küçük.\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " '%s' seçeneği bir tam sayı gerektirir.\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " '%s' seçeneği pozitif bir değer gerektirir.\n",
        MSG_ERRO "[ ❌ HATA 400 ]" RESET " Süre negatif olamaz.\n",
        MSG_ERRO "[ ❌ HATA 206 ]" RESET " Bellek ayrılamadı (wcsdup).\n",
        MSG_AVISO "[ ⚠️ UYARI 403 ]" RESET " Arabellek 32MB ile sınırlandırıldı. Görüntü bozulabilir.\n",
        MSG_AVISO "[ ⚠️ UYARI 203 ]" RESET " Özel anahtar yüklenemedi, yerleşik anahtar kullanılıyor.\n",
        MSG_CMD_DIM "[ ⚙️ HATA AYIKLAMA 214 ]" RESET " Geometrik hizasızlık tespit edildi. Stabilizasyon değişebilir.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [11] POLONÊS (PL) ----------------
    {
        MSG_ERRO "Błąd podczas otwierania pliku\n" RESET,
        "Oryginalny Twórca: ",
        "Skompilowane przez: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ZMODYFIKOWANO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Zaawansowany Upiększacz Terminala" RESET "\n",
        "Użycie: " BG_FOSCO " cat plik | neonx [opcje] " RESET "\n\n"
        "  -m [0-11]          Ustawia styl animacji\n"
        "  -s [wartość]       Prędkość przejścia " MSG_CMD_DIM "(Domyślnie: 0.2)" RESET "\n"
        "  -f [wartość]       Częstotliwość fali " MSG_CMD_DIM "(Domyślnie: 0.3)" RESET "\n"
        "  -d [wartość]       Czas trwania w sekundach " MSG_CMD_DIM "(0 = Nieskończoność)" RESET "\n"
        "  -max-lines [war]   Maksymalny limit linii " MSG_CMD_DIM "(Domyślnie: 10.000)" RESET "\n"
        "  -A [stopnie]       Obraca kąt gradientu " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [wartość]   Ustawia stały seed " MSG_CMD_DIM "(Deterministyczny)" RESET "\n"
        "  -S                 Renderuje statyczną klatkę " MSG_CMD_DIM "(Bez animacji)" RESET "\n"
        "  -c [szerokość]     Wymusza statyczną szerokość dla gradientu\n"
        "  -o [0-1]           Dostosowuje poziomą przezroczystość/gładkość\n"
        "  -O [0-1]           Włącza Pionową Przezroczystość (zanikanie góra/dół)\n"
        "  -F [wartość]       Blokuje liczbę klatek na sekundę " MSG_CMD_DIM "(np. 60, 90)" RESET "\n"
        "  -L                 Przetwarzanie linia po linii " MSG_CMD_DIM "(Strumień)" RESET "\n"
        "  --fo [0-1]         Włącza Tryb Matowy (Zmniejsza żywość)\n"
        "  --preset [nazwa]   Ładuje palety " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias dla --color1 i --color2\n"
        "  --color1 [hex]     Ustawia kolor początkowy gradientu (np. #FF0000)\n"
        "  --color2 [hex]     Ustawia kolor końcowy gradientu (np. #FFA500)\n"
        "  --quantized        Kwantyzacja kolorów " MSG_CMD_DIM "(Wyższa wydajność)" RESET "\n"
        "  --no-ansi          Wyłącza użycie kolorów ANSI w wyjściu\n"
        "  --spin             Generuje czyste kody ANSI " MSG_CMD_DIM "(Dla skryptów)" RESET "\n"
        "  --lang [id]        Nadpisuje język " MSG_CMD_DIM "(np. pl, en)" RESET "\n"
        "  --license          Wyświetla warunki licencji\n"
        "  -v, --version      Pokazuje wersję i status pliku binarnego\n"
        "  -h, --help         Wyświetla ten interaktywny panel pomocy\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Opcja '%s' wymaga podania wartości liczbowej.\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Opcja '%s' wymaga liczby. Otrzymano: '%s'\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Tryb animacji (-m) musi być liczbą całkowitą.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " Tryb animacji (-m) musi zawierać się w przedziale 0-11.\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Nieprawidłowa opcja lub luźny argument '%s'\n",
        "\n" MSG_ERRO "[ ❌ BŁĄD 404 ]" RESET " Nie przekazano żadnych danych do pliku binarnego!\n",
        "\n" MSG_ERRO "[ ❌ BŁĄD 413 ]" RESET " Plik jest za duży. Użyj -L dla trybu stream.\n",
        MSG_AVISO "[ ⚠️ OSTRZ 403 ]" RESET " Nie można zweryfikować integralności (ograniczony system).\n",
        MSG_ERRO "[ ❌ BŁĄD 403 ]" RESET " Nie udało się otworzyć pliku wykonalnego do weryfikacji.\n",
        MSG_ERRO "[ ❌ BŁĄD 403 ]" RESET " Błąd odczytu podczas weryfikacji integralności.\n",
        MSG_ERRO "[ ❌ BŁĄD 403 ]" RESET " Niewystarczająca ilość pamięci do weryfikacji.\n",
        MSG_ERRO "[ ❌ BŁĄD 403 ]" RESET " Nieprawidłowy podpis lub błędny format szesnastkowy.\n",
        MSG_ERRO "[ ❌ BŁĄD 403 ]" RESET " Plik jest za mały, aby zawierać podpis.\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Opcja '%s' wymaga liczby całkowitej.\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Opcja '%s' wymaga wartości dodatniej.\n",
        MSG_ERRO "[ ❌ BŁĄD 400 ]" RESET " Czas trwania nie może być ujemny.\n",
        MSG_ERRO "[ ❌ BŁĄD 206 ]" RESET " Nie można przydzielić pamięci (wcsdup).\n",
        MSG_AVISO "[ ⚠️ OSTRZ 403 ]" RESET " Bufor obcięty do 32MB. Renderowanie może ulec uszkodzeniu.\n",
        MSG_AVISO "[ ⚠️ OSTRZ 203 ]" RESET " Błąd ładowania własnego klucza, używam wbudowanego.\n",
        MSG_CMD_DIM "[ ⚙️ DEBUGOWANIE 214 ]" RESET " Wykryto przesunięcie geometryczne. Stabilizacja może ulec zmianie.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [12] INDONÉSIO (ID) ----------------
    {
        MSG_ERRO "Kesalahan saat membuka file\n" RESET,
        "Pembuat Asli: ",
        "Dikompalasi oleh: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "DIMODIFIKASI" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Peningkat Terminal Lanjutan" RESET "\n",
        "Penggunaan: " BG_FOSCO " cat file | neonx [opsi] " RESET "\n\n"
        "  -m [0-11]          Menentukan gaya animasi\n"
        "  -s [nilai]         Kecepatan transisi " MSG_CMD_DIM "(Bawaan: 0.2)" RESET "\n"
        "  -f [nilai]         Frekuensi gelombang " MSG_CMD_DIM "(Bawaan: 0.3)" RESET "\n"
        "  -d [nilai]         Durasi dalam detik " MSG_CMD_DIM "(0 = Tak terbatas)" RESET "\n"
        "  -max-lines [nil]   Batas garis maksimum " MSG_CMD_DIM "(Bawaan: 10.000)" RESET "\n"
        "  -A [derajat]       Memutar sudut gradien " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [nilai]     Menentukan seed tetap " MSG_CMD_DIM "(Deterministik)" RESET "\n"
        "  -S                 Menyajikan bingkai statis " MSG_CMD_DIM "(Tanpa animasi)" RESET "\n"
        "  -c [lebar]         Memaksa lebar statis untuk gradien\n"
        "  -o [0-1]           Menyesuaikan opasitas/kehalusan horizontal\n"
        "  -O [0-1]           Mengaktifkan Opasitas Vertikal (pemudaran atas/bawah)\n"
        "  -F [nilai]         Mengunci kecepatan bingkai " MSG_CMD_DIM "(mis: 60, 90)" RESET "\n"
        "  -L                 Pemrosesan baris demi baris " MSG_CMD_DIM "(Aliran)" RESET "\n"
        "  --fo [0-1]         Mengaktifkan Mode Matte (Mengurangi kecerahan)\n"
        "  --preset [nama]    Memuat palet " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Alias untuk --color1 dan --color2\n"
        "  --color1 [hex]     Menentukan warna awal gradien (mis: #FF0000)\n"
        "  --color2 [hex]     Menentukan warna akhir gradien (mis: #FFA500)\n"
        "  --quantized        Kuantisasi warna " MSG_CMD_DIM "(Performa lebih tinggi)" RESET "\n"
        "  --no-ansi          Menonaktifkan penggunaan warna ANSI pada keluaran\n"
        "  --spin             Menghasilkan kode ANSI murni " MSG_CMD_DIM "(Untuk skrip)" RESET "\n"
        "  --lang [id]        Mengesampingkan bahasa " MSG_CMD_DIM "(mis: id, en)" RESET "\n"
        "  --license          Menampilkan persyaratan lisensi\n"
        "  -v, --version      Menampilkan versi dan status biner\n"
        "  -h, --help         Menampilkan panel bantuan interaktif ini\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Opsi '%s' memerlukan nilai numerik setelahnya.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Opsi '%s' memerlukan angka. Diterima: '%s'\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Mode animasi (-m) harus berupa bilangan bulat.\n",
        MSG_INFO "[ ℹ️ INFO 416 ]" RESET " Mode animasi (-m) harus antara 0 dan 11.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Opsi tidak valid atau argumen lepas '%s'\n",
        "\n" MSG_ERRO "[ ❌ ERROR 404 ]" RESET " Tidak ada data yang diteruskan ke biner!\n",
        "\n" MSG_ERRO "[ ❌ ERROR 413 ]" RESET " File terlalu besar. Gunakan -L untuk mode stream.\n",
        MSG_AVISO "[ ⚠️ AWAS 403 ]" RESET " Tidak dapat memverifikasi integritas (sistem dibatasi).\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Gagal membuka eksekutabel untuk verifikasi integritas.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Kesalahan membaca file selama verifikasi integritas.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Memori tidak cukup untuk memverifikasi integritas.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " Tanda tangan tidak valid atau format heksadesimal salah.\n",
        MSG_ERRO "[ ❌ ERROR 403 ]" RESET " File terlalu kecil untuk menampung tanda tangan.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Opsi '%s' memerlukan bilangan bulat.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Opsi '%s' memerlukan nilai positif.\n",
        MSG_ERRO "[ ❌ ERROR 400 ]" RESET " Durasi tidak boleh negatif.\n",
        MSG_ERRO "[ ❌ ERROR 206 ]" RESET " Tidak dapat mengalokasikan memori (wcsdup).\n",
        MSG_AVISO "[ ⚠️ AWAS 403 ]" RESET " Buffer dipotong menjadi 32MB. Render bisa rusak.\n",
        MSG_AVISO "[ ⚠️ AWAS 203 ]" RESET " Gagal memuat kunci kustom, menggunakan kunci bawaan.\n",
        MSG_CMD_DIM "[ ⚙️ DEBUG 214 ]" RESET " Ketidaksejajaran geometris terdeteksi. Stabilisasi mungkin bervariasi.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [13] ÁRABE (AR) ----------------
    {
        MSG_ERRO "خطأ في فتح الملف\n" RESET,
        "المبتكر الأصلي: ",
        "تم التجميع بواسطة: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "تم التعديل" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - مُجمّل الطرفية المتقدم" RESET "\n",
        "الاستخدام: " BG_FOSCO " cat ملف | neonx [خيارات] " RESET "\n\n"
        "  -m [0-11]          تحديد نمط الحركة\n"
        "  -s [قيمة]          سرعة الانتقال " MSG_CMD_DIM "(الافتراضي: 0.2)" RESET "\n"
        "  -f [قيمة]          تردد الموجة " MSG_CMD_DIM "(الافتراضي: 0.3)" RESET "\n"
        "  -d [قيمة]          المدة بالثواني " MSG_CMD_DIM "(0 = لا نهائي)" RESET "\n"
        "  -max-lines [قيمة]  الحد الأقصى للأسطر " MSG_CMD_DIM "(الافتراضي: 10.000)" RESET "\n"
        "  -A [درجات]         تدوير زاوية التدرج " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [قيمة]      تحديد بذرة ثابتة " MSG_CMD_DIM "(حتمي)" RESET "\n"
        "  -S                 عرض إطار ثابت " MSG_CMD_DIM "(بدون حركة)" RESET "\n"
        "  -c [عرض]           فرض عرض ثابت للتدرج\n"
        "  -o [0-1]           ضبط الشفافية/النعومة الأفقية\n"
        "  -O [0-1]           تفعيل الشفافية العمودية (تلاشي أعلى/أسفل)\n"
        "  -F [قيمة]          قفل معدل الإطارات " MSG_CMD_DIM "(مثال: 60, 90)" RESET "\n"
        "  -L                 معالجة سطر بسطر " MSG_CMD_DIM "(تدفق)" RESET "\n"
        "  --fo [0-1]         تفعيل الوضع غير اللامع (يقلل من الحيوية)\n"
        "  --preset [اسم]     تحميل لوحات الألوان " MSG_CMD_DIM "(cyberpunk, retro...)" RESET "\n"
        "  --c1, --c2 [hex]   اختصارات لـ --color1 و --color2\n"
        "  --color1 [hex]     تحديد لون البداية للتدرج (مثال: #FF0000)\n"
        "  --color2 [hex]     تحديد لون النهاية للتدرج (مثال: #FFA500)\n"
        "  --quantized        تكميم الألوان " MSG_CMD_DIM "(أداء أعلى)" RESET "\n"
        "  --no-ansi          تعطيل ألوان ANSI في المخرجات\n"
        "  --spin             إنشاء أكواد ANSI نقية " MSG_CMD_DIM "(للبرامج النصية)" RESET "\n"
        "  --lang [id]        تجاوز اللغة " MSG_CMD_DIM "(مثال: ar, en)" RESET "\n"
        "  --license          عرض شروط الترخيص\n"
        "  -v, --version      عرض الإصدار وحالة الملف التنفيذي\n"
        "  -h, --help         عرض لوحة المساعدة التفاعلية هذه\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " الخيار '%s' يتطلب قيمة رقمية بعده.\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " الخيار '%s' يتطلب رقمًا. تم الاستلام: '%s'\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " وضع الرسوم المتحركة (-m) يجب أن يكون عددًا صحيحًا.\n",
        MSG_INFO "[ ℹ️ معلومات 416 ]" RESET " وضع الرسوم المتحركة (-m) يجب أن يكون بين 0 و 11.\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " خيار غير صالح أو وسيطة مفقودة '%s'\n",
        "\n" MSG_ERRO "[ ❌ خطأ 404 ]" RESET " لم يتم تمرير أي بيانات إلى الثنائي!\n",
        "\n" MSG_ERRO "[ ❌ خطأ 413 ]" RESET " الملف كبير جدًا. استخدم -L لوضع البث.\n",
        MSG_AVISO "[ ⚠️ تحذير 403 ]" RESET " تعذر التحقق من النزاهة (نظام مقيد).\n",
        MSG_ERRO "[ ❌ خطأ 403 ]" RESET " فشل فتح الملف التنفيذي للتحقق من النزاهة.\n",
        MSG_ERRO "[ ❌ خطأ 403 ]" RESET " خطأ في قراءة الملف أثناء التحقق من النزاهة.\n",
        MSG_ERRO "[ ❌ خطأ 403 ]" RESET " ذاكرة غير كافية للتحقق من النزاهة.\n",
        MSG_ERRO "[ ❌ خطأ 403 ]" RESET " توقيع غير صالح أو تنسيق سداسي عشري غير صحيح.\n",
        MSG_ERRO "[ ❌ خطأ 403 ]" RESET " الملف صغير جدًا بحيث لا يحتوي على التوقيع.\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " الخيار '%s' يتطلب عددًا صحيحًا.\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " الخيار '%s' يتطلب قيمة موجبة.\n",
        MSG_ERRO "[ ❌ خطأ 400 ]" RESET " لا يمكن أن تكون المدة سالبة.\n",
        MSG_ERRO "[ ❌ خطأ 206 ]" RESET " تعذر تخصيص الذاكرة (wcsdup).\n",
        MSG_AVISO "[ ⚠️ تحذير 403 ]" RESET " تم اقتطاع المخزن المؤقت إلى 32 ميغابايت. قد يتلف العرض.\n",
        MSG_AVISO "[ ⚠️ تحذير 203 ]" RESET " فشل تحميل المفتاح المخصص، جاري استخدام المدمج.\n",
        MSG_CMD_DIM "[ ⚙️ تصحيح 214 ]" RESET " تم اكتشاف عدم محاذاة هندسية. قد يختلف الاستقرار.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [14] BÚLGARO (BG) ----------------
    {
        MSG_ERRO "Грешка при отваряне на файл\n" RESET,
        "Оригинален създател: ",
        "Компилирано от: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "МОДИФИЦИРАН" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Разширен Разкрасител на Терминала" RESET "\n",
        "Употреба: " BG_FOSCO " cat файл | neonx [опции] " RESET "\n\n"
        "  -m [0-11]          Задава стила на анимацията\n"
        "  -s [стойност]      Скорост на прехода " MSG_CMD_DIM "(По подразбиране: 0.2)" RESET "\n"
        "  -f [стойност]      Честота на вълната " MSG_CMD_DIM "(По подразбиране: 0.3)" RESET "\n"
        "  -d [стойност]      Продължителност в секунди " MSG_CMD_DIM "(0 = Безкрайно)" RESET "\n"
        "  -max-lines [ст]    Максимален лимит на редовете " MSG_CMD_DIM "(По подразбиране: 10.000)" RESET "\n"
        "  -A [градуси]       Завърта ъгъла на градиента " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [стойност]  Задава фиксиран seed " MSG_CMD_DIM "(Детерминирано)" RESET "\n"
        "  -S                 Рендерира статичен кадър " MSG_CMD_DIM "(Без анимация)" RESET "\n"
        "  -c [ширина]        Налага статична ширина за градиента\n"
        "  -o [0-1]           Регулира хоризонталната непрозрачност/гладкост\n"
        "  -O [0-1]           Активира вертикална непрозрачност (избледняване горе/долу)\n"
        "  -F [стойност]      Заключва кадровата честота " MSG_CMD_DIM "(напр: 60, 90)" RESET "\n"
        "  -L                 Обработка ред по ред " MSG_CMD_DIM "(Поток)" RESET "\n"
        "  --fo [0-1]         Активира Матов Режим (Намалява яркостта)\n"
        "  --preset [име]     Зарежда палитри " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Псевдоними за --color1 и --color2\n"
        "  --color1 [hex]     Задава начален цвят на градиента (напр: #FF0000)\n"
        "  --color2 [hex]     Задава краен цвят на градиента (напр: #FFA500)\n"
        "  --quantized        Квантуване на цветове " MSG_CMD_DIM "(По-висока производителност)" RESET "\n"
        "  --no-ansi          Деактивира ANSI цветове в изхода\n"
        "  --spin             Генерира чисти ANSI кодове " MSG_CMD_DIM "(За скриптове)" RESET "\n"
        "  --lang [id]        Замества езика " MSG_CMD_DIM "(напр: bg, en)" RESET "\n"
        "  --license          Показва лицензионните условия\n"
        "  -v, --version      Показва версията и статуса на бинарния файл\n"
        "  -h, --help         Показва този интерактивен помощен панел\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Опцията '%s' изисква числова стойност след нея.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Опцията '%s' изисква число. Получено: '%s'\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Режимът на анимация (-m) трябва да е цяло число.\n",
        MSG_INFO "[ ℹ️ ИНФО 416 ]" RESET " Режимът на анимация (-m) трябва да е между 0 и 11.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Невалидна опция или свободен аргумент '%s'\n",
        "\n" MSG_ERRO "[ ❌ ГРЕШКА 404 ]" RESET " Няма подадени данни към бинарния файл!\n",
        "\n" MSG_ERRO "[ ❌ ГРЕШКА 413 ]" RESET " Файлът е твърде голям. Използвайте -L за stream режим.\n",
        MSG_AVISO "[ ⚠️ ВНИМАНИЕ 403 ]" RESET " Цялостта не може да бъде проверена (ограничена система).\n",
        MSG_ERRO "[ ❌ ГРЕШКА 403 ]" RESET " Неуспешно отваряне на изпълнимия файл за проверка.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 403 ]" RESET " Грешка при четене на файла по време на проверка.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 403 ]" RESET " Недостатъчна памет за проверка на целостта.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 403 ]" RESET " Невалиден подпис или грешен шестнадесетичен формат.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 403 ]" RESET " Файлът е твърде малък, за да съдържа подпис.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Опцията '%s' изисква цяло число.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Опцията '%s' изисква положителна стойност.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 400 ]" RESET " Продължителността не може да бъде отрицателна.\n",
        MSG_ERRO "[ ❌ ГРЕШКА 206 ]" RESET " Не може да се задели памет (wcsdup).\n",
        MSG_AVISO "[ ⚠️ ВНИМАНИЕ 403 ]" RESET " Буферът е отрязан до 32MB. Рендерирането може да се повреди.\n",
        MSG_AVISO "[ ⚠️ ВНИМАНИЕ 203 ]" RESET " Неуспешно зареждане на персонализиран ключ, използва се вграденият.\n",
        MSG_CMD_DIM "[ ⚙️ ДЕБЪГ 214 ]" RESET " Открито е геометрично разминаване. Стабилизацията може да варира.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [15] GREGO (EL) ----------------
    {
        MSG_ERRO "Σφάλμα κατά το άνοιγμα του αρχείου\n" RESET,
        "Αρχικός Δημιουργός: ",
        "Μεταγλωττίστηκε από: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ΤΡΟΠΟΠΟΙΗΜΕΝΟ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Προηγμένο Εργαλείο Καλλωπισμού Τερματικού" RESET "\n",
        "Χρήση: " BG_FOSCO " cat αρχείο | neonx [επιλογές] " RESET "\n\n"
        "  -m [0-11]          Ορίζει το στυλ της κίνησης\n"
        "  -s [τιμή]          Ταχύτητα μετάβασης " MSG_CMD_DIM "(Προεπιλογή: 0.2)" RESET "\n"
        "  -f [τιμή]          Συχνότητα κύματος " MSG_CMD_DIM "(Προεπιλογή: 0.3)" RESET "\n"
        "  -d [τιμή]          Διάρκεια σε δευτερόλεπτα " MSG_CMD_DIM "(0 = Άπειρο)" RESET "\n"
        "  -max-lines [τιμή]  Μέγιστο όριο γραμμών " MSG_CMD_DIM "(Προεπιλογή: 10.000)" RESET "\n"
        "  -A [μοίρες]        Περιστρέφει τη γωνία του граδιέντ " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [τιμή]      Ορίζει ένα σταθερό seed " MSG_CMD_DIM "(Ντετερμινιστικό)" RESET "\n"
        "  -S                 Αποδίδει ένα στατικό καρέ " MSG_CMD_DIM "(Χωρίς κίνηση)" RESET "\n"
        "  -c [πλάτος]        Επιβάλλει στατικό πλάτος για το граδιέντ\n"
        "  -o [0-1]           Ρυθμίζει την οριζόντια αδιαφάνεια/ομαλότητα\n"
        "  -O [0-1]           Ενεργοποιεί Κάθετη Αδιαφάνεια (ξεθώριασμα πάνω/κάτω)\n"
        "  -F [τιμή]          Κλειδώνει το ρυθμό καρέ " MSG_CMD_DIM "(π.χ.: 60, 90)" RESET "\n"
        "  -L                 Επεξεργασία γραμμή προς γραμμή " MSG_CMD_DIM "(Ροή)" RESET "\n"
        "  --fo [0-1]         Ενεργοποιεί Ματ Λειτουργία (Μειώνει τη ζωντάνια)\n"
        "  --preset [όνομα]   Φορτώνει παλέτες " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   Ψευδώνυμο για --color1 και --color2\n"
        "  --color1 [hex]     Ορίζει το αρχικό χρώμα του граδιέντ (π.χ.: #FF0000)\n"
        "  --color2 [hex]     Ορίζει το τελικό χρώμα του граδιέντ (π.χ.: #FFA500)\n"
        "  --quantized        Κβαντισμός χρωμάτων " MSG_CMD_DIM "(Υψηλότερη απόδοση)" RESET "\n"
        "  --no-ansi          Απενεργοποιεί τη χρήση χρωμάτων ANSI στην έξοδο\n"
        "  --spin             Παράγει καθαρούς κώδικες ANSI " MSG_CMD_DIM "(Για σενάρια)" RESET "\n"
        "  --lang [id]        Παρακάμπτει τη γλώσσα " MSG_CMD_DIM "(π.χ.: el, en)" RESET "\n"
        "  --license          Εμφανίζει τους όρους άδειας χρήσης\n"
        "  -v, --version      Εμφανίζει την έκδοση και την κατάσταση του εκτελέσιμου\n"
        "  -h, --help         Εμφανίζει αυτόν τον διαδραστικό πίνακα βοήθειας\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Η επιλογή '%s' απαιτεί μια αριθμητική τιμή.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Η επιλογή '%s' απαιτεί αριθμό. Λήφθηκε: '%s'\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Η λειτουργία κιν. σχεδίων (-m) πρέπει να είναι ακέραιος.\n",
        MSG_INFO "[ ℹ️ ΠΛΗΡΟΦΟΡΙΑ 416 ]" RESET " Η λειτουργία κιν. σχεδίων (-m) πρέπει να είναι μεταξύ 0 και 11.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Μη έγκυρη επιλογή ή ορφανό όρισμα '%s'\n",
        "\n" MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 404 ]" RESET " Δεν διαβιβάστηκαν δεδομένα στο εκτελέσιμο!\n",
        "\n" MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 413 ]" RESET " Αρχείο πολύ μεγάλο. Χρησιμοποιήστε -L για λειτουργία stream.\n",
        MSG_AVISO "[ ⚠️ ΠΡΟΕΙΔ 403 ]" RESET " Αδυναμία επαλήθευσης ακεραιότητας (περιορισμένο σύστημα).\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 403 ]" RESET " Αποτυχία ανοίγματος του εκτελέσιμου για επαλήθευση.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 403 ]" RESET " Σφάλμα ανάγνωσης αρχείου κατά την επαλήθευση ακεραιότητας.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 403 ]" RESET " Ανεπαρκής μνήμη για την επαλήθευση της ακεραιότητας.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 403 ]" RESET " Μη έγκυρη υπογραφή ή λανθασμένη δεκαεξαδική μορφή.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 403 ]" RESET " Το αρχείο είναι πολύ μικρό για να περιέχει την υπογραφή.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Η επιλογή '%s' απαιτεί ακέραιο αριθμό.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Η επιλογή '%s' απαιτεί θετική τιμή.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 400 ]" RESET " Η διάρκεια δεν μπορεί να είναι αρνητική.\n",
        MSG_ERRO "[ ❌ ΣΦΑΛΜΑ 206 ]" RESET " Αδυναμία εκχώρησης μνήμης (wcsdup).\n",
        MSG_AVISO "[ ⚠️ ΠΡΟΕΙΔ 403 ]" RESET " Η μνήμη buffer περικόπηκε στα 32MB. Η απόδοση μπορεί να αλλοιωθεί.\n",
        MSG_AVISO "[ ⚠️ ΠΡΟΕΙΔ 203 ]" RESET " Αποτυχία φόρτωσης προσαρμοσμένου κλειδιού, χρήση ενσωματωμένου.\n",
        MSG_CMD_DIM "[ ⚙️ ΕΚΣΦΑΛΜΑΤΩΣΗ 214 ]" RESET " Ανιχνεύτηκε γεωμετρική απόκλιση. Η σταθεροποίηση μπορεί να διαφέρει.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [16] HINDI (HI) ----------------
    {
        MSG_ERRO "फ़ाइल खोलने में त्रुटि\n" RESET,
        "मूल निर्माता: ",
        "द्वारा संकलित: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "संशोधित" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - उन्नत टर्मिनल सौंदर्य उपकरण" RESET "\n",
        "उपयोग: " BG_FOSCO " cat फ़ाइल | neonx [विकल्प] " RESET "\n\n"
        "  -m [0-11]          एनीमेशन शैली सेट करता है\n"
        "  -s [मान]           संक्रमण गति " MSG_CMD_DIM "(डिफ़ॉल्ट: 0.2)" RESET "\n"
        "  -f [मान]           तरंग आवृत्ति " MSG_CMD_DIM "(डिफ़ॉल्ट: 0.3)" RESET "\n"
        "  -d [मान]           सेकंड में अवधि " MSG_CMD_DIM "(0 = अनंत)" RESET "\n"
        "  -max-lines [मान]   अधिकतम पंक्ति सीमा " MSG_CMD_DIM "(डिफ़ॉल्ट: 10.000)" RESET "\n"
        "  -A [डिग्री]        ग्रैडिएंट कोण घुमाता है " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [मान]       निश्चित बीज सेट करता है " MSG_CMD_DIM "(नियतिवादी)" RESET "\n"
        "  -S                 स्थिर फ्रेम रेंडर करता है " MSG_CMD_DIM "(बिना एनीमेशन)" RESET "\n"
        "  -c [चौड़ाई]        ग्रैडिएंट के लिए स्थिर चौड़ाई लागू करता है\n"
        "  -o [0-1]           क्षैतिज अपारदर्शिता/चिकनाई समायोजित करता है\n"
        "  -O [0-1]           लंबवत अपारदर्शिता सक्षम करता है (ऊपर/नीचे फेडिंग)\n"
        "  -F [मान]           फ्रेम रेट लॉक करता है " MSG_CMD_DIM "(उदा: 60, 90)" RESET "\n"
        "  -L                 पंक्ति दर पंक्ति प्रसंस्करण " MSG_CMD_DIM "(स्ट्रीम)" RESET "\n"
        "  --fo [0-1]         मैट मोड सक्षम करता है (चमक कम करता है)\n"
        "  --preset [नाम]     पैलेट लोड करता है " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   --color1 और --color2 के लिए उपनाम\n"
        "  --color1 [hex]     ग्रैडिएंट का प्रारंभिक रंग सेट करता है (उदा: #FF0000)\n"
        "  --color2 [hex]     ग्रैडिएंट का अंतिम रंग सेट करता है (उदा: #FFA500)\n"
        "  --quantized        रंग परिमाणीकरण " MSG_CMD_DIM "(उच्च प्रदर्शन)" RESET "\n"
        "  --no-ansi          आउटपुट में ANSI रंगों का उपयोग अक्षम करता है\n"
        "  --spin             शुद्ध ANSI कोड उत्पन्न करता है " MSG_CMD_DIM "(स्क्रिप्ट के लिए)" RESET "\n"
        "  --lang [id]        भाषा ओवरराइड करता है " MSG_CMD_DIM "(उदा: hi, en)" RESET "\n"
        "  --license          लाइसेंस शर्तें दिखाता है\n"
        "  -v, --version      बाइनरी का संस्करण और स्थिति दिखाता है\n"
        "  -h, --help         यह इंटरैक्टिव सहायता पैनल दिखाता है\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " विकल्प '%s' के बाद संख्यात्मक मान की आवश्यकता है।\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " विकल्प '%s' के लिए एक संख्या आवश्यक है। प्राप्त: '%s'\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " एनीमेशन मोड (-m) एक पूर्णांक होना चाहिए।\n",
        MSG_INFO "[ ℹ️ जानकारी 416 ]" RESET " एनीमेशन मोड (-m) 0 और 11 के बीच होना चाहिए।\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " अमान्य विकल्प या मुक्त तर्क '%s'\n",
        "\n" MSG_ERRO "[ ❌ त्रुटि 404 ]" RESET " बाइनरी में कोई डेटा पास नहीं किया गया!\n",
        "\n" MSG_ERRO "[ ❌ त्रुटि 413 ]" RESET " फ़ाइल बहुत बड़ी है। स्ट्रीम मोड के लिए -L का उपयोग करें।\n",
        MSG_AVISO "[ ⚠️ चेतावनी 403 ]" RESET " अखंडता सत्यापित नहीं की जा सकी (प्रतिबंधित प्रणाली)।\n",
        MSG_ERRO "[ ❌ त्रुटि 403 ]" RESET " अखंडता सत्यापन के लिए निष्पादन योग्य खोलने में विफल।\n",
        MSG_ERRO "[ ❌ त्रुटि 403 ]" RESET " अखंडता सत्यापन के दौरान फ़ाइल पढ़ने में त्रुटि।\n",
        MSG_ERRO "[ ❌ त्रुटि 403 ]" RESET " अखंडता सत्यापित करने के लिए अपर्याप्त मेमोरी।\n",
        MSG_ERRO "[ ❌ त्रुटि 403 ]" RESET " अमान्य हस्ताक्षर या गलत हेक्साडेसिमल प्रारूप।\n",
        MSG_ERRO "[ ❌ त्रुटि 403 ]" RESET " हस्ताक्षर शामिल करने के लिए फ़ाइल बहुत छोटी है।\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " विकल्प '%s' के लिए पूर्णांक की आवश्यकता है।\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " विकल्प '%s' के लिए सकारात्मक मान की आवश्यकता है।\n",
        MSG_ERRO "[ ❌ त्रुटि 400 ]" RESET " अवधि नकारात्मक नहीं हो सकती।\n",
        MSG_ERRO "[ ❌ त्रुटि 206 ]" RESET " मेमोरी आवंटित नहीं की जा सकी (wcsdup)।\n",
        MSG_AVISO "[ ⚠️ चेतावनी 403 ]" RESET " बफ़र 32MB तक छोटा कर दिया गया। प्रतिपादन दूषित हो सकता है।\n",
        MSG_AVISO "[ ⚠️ चेतावनी 203 ]" RESET " कस्टम कुंजी लोड करने में विफल, अंतर्निहित का उपयोग कर रहा है।\n",
        MSG_CMD_DIM "[ ⚙️ डीबग 214 ]" RESET " ज्यामितीय संरेखण त्रुटि पाई गई। स्थिरीकरण भिन्न हो सकता है।\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [17] TAILANDÊS (TH) ----------------
    {
        MSG_ERRO "ข้อผิดพลาดในการเปิดไฟล์\n" RESET,
        "ผู้สร้างดั้งเดิม: ",
        "คอมไพล์โดย: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ถูกปรับเปลี่ยน" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - เครื่องมือปรับแต่งเทอร์มินัลขั้นสูง" RESET "\n",
        "การใช้งาน: " BG_FOSCO " cat ไฟล์ | neonx [ตัวเลือก] " RESET "\n\n"
        "  -m [0-11]          กำหนดสไตล์แอนิเมชัน\n"
        "  -s [ค่า]             ความเร็วในการเปลี่ยนผ่าน " MSG_CMD_DIM "(ค่าเริ่มต้น: 0.2)" RESET "\n"
        "  -f [ค่า]             ความถี่ของคลื่น " MSG_CMD_DIM "(ค่าเริ่มต้น: 0.3)" RESET "\n"
        "  -d [ค่า]             ระยะเวลาในหน่วยวินาที " MSG_CMD_DIM "(0 = อนันต์)" RESET "\n"
        "  -max-lines [ค่า]     ขีดจำกัดบรรทัดสูงสุด " MSG_CMD_DIM "(ค่าเริ่มต้น: 10.000)" RESET "\n"
        "  -A [องศา]          หมุนมุมของการไล่ระดับสี " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [ค่า]         กำหนดซีดคงที่ " MSG_CMD_DIM "(เชิงกำหนด)" RESET "\n"
        "  -S                 เรนเดอร์เฟรมแบบคงที่ " MSG_CMD_DIM "(ไม่มีแอนิเมชัน)" RESET "\n"
        "  -c [ความกว้าง]      บังคับความกว้างแบบคงที่สำหรับการไล่ระดับสี\n"
        "  -o [0-1]           ปรับความทึบแสง/ความเรียบแนวนอน\n"
        "  -O [0-1]           เปิดใช้งานความทึบแสงแนวตั้ง (เฟดดิงบน/ล่าง)\n"
        "  -F [ค่า]             ล็อคอัตราเฟรม " MSG_CMD_DIM "(เช่น: 60, 90)" RESET "\n"
        "  -L                 ประมวลผลทีละบรรทัด " MSG_CMD_DIM "(สตรีม)" RESET "\n"
        "  --fo [0-1]         เปิดใช้งานโหมดด้าน (ลดความสดใส)\n"
        "  --preset [ชื่อ]      โหลดพาเลตต์ " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        "  --c1, --c2 [hex]   นามแฝงสำหรับ --color1 และ --color2\n"
        "  --color1 [hex]     กำหนดสีเริ่มต้นของการไล่ระดับสี (เช่น: #FF0000)\n"
        "  --color2 [hex]     กำหนดสีสิ้นสุดของการไล่ระดับสี (เช่น: #FFA500)\n"
        "  --quantized        การควอนไทซ์สี " MSG_CMD_DIM "(ประสิทธิภาพสูงขึ้น)" RESET "\n"
        "  --no-ansi          ปิดใช้งานการใช้สี ANSI ในเอาต์พุต\n"
        "  --spin             สร้างรหัส ANSI ล้วน " MSG_CMD_DIM "(สำหรับสคริปต์)" RESET "\n"
        "  --lang [id]        แทนที่ภาษา " MSG_CMD_DIM "(เช่น: th, en)" RESET "\n"
        "  --license          แสดงเงื่อนไขการอนุญาตให้ใช้สิทธิ\n"
        "  -v, --version      แสดงเวอร์ชันและสถานะของไบนารี\n"
        "  -h, --help         แสดงแผงความช่วยเหลือแบบโต้ตอบนี้\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " ตัวเลือก '%s' ต้องการค่าตัวเลขต่อท้าย\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " ตัวเลือก '%s' ต้องการตัวเลข ได้รับ: '%s'\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " โหมดแอนิเมชัน (-m) ต้องเป็นจำนวนเต็ม\n",
        MSG_INFO "[ ℹ️ ข้อมูล 416 ]" RESET " โหมดแอนิเมชัน (-m) ต้องอยู่ระหว่าง 0 ถึง 11\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " ตัวเลือกไม่ถูกต้องหรืออาร์กิวเมนต์หลุด '%s'\n",
        "\n" MSG_ERRO "[ ❌ ข้อผิดพลาด 404 ]" RESET " ไม่มีการส่งผ่านข้อมูลไปยังไบนารี!\n",
        "\n" MSG_ERRO "[ ❌ ข้อผิดพลาด 413 ]" RESET " ไฟล์ใหญ่เกินไป ใช้ -L สำหรับโหมดสตรีม\n",
        MSG_AVISO "[ ⚠️ คำเตือน 403 ]" RESET " ไม่สามารถตรวจสอบความสมบูรณ์ได้ (ระบบถูกจำกัด)\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 403 ]" RESET " ล้มเหลวในการเปิดไฟล์ปฏิบัติการเพื่อตรวจสอบความสมบูรณ์\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 403 ]" RESET " ข้อผิดพลาดในการอ่านไฟล์ระหว่างการตรวจสอบความสมบูรณ์\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 403 ]" RESET " หน่วยความจำไม่เพียงพอที่จะตรวจสอบความสมบูรณ์\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 403 ]" RESET " ลายเซ็นไม่ถูกต้องหรือรูปแบบเลขฐานสิบหกไม่ถูกต้อง\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 403 ]" RESET " ไฟล์เล็กเกินไปที่จะมีลายเซ็น\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " ตัวเลือก '%s' ต้องการจำนวนเต็ม\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " ตัวเลือก '%s' ต้องการค่าบวก\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 400 ]" RESET " ระยะเวลาไม่สามารถติดลบได้\n",
        MSG_ERRO "[ ❌ ข้อผิดพลาด 206 ]" RESET " ไม่สามารถจัดสรรหน่วยความจำได้ (wcsdup)\n",
        MSG_AVISO "[ ⚠️ คำเตือน 403 ]" RESET " บัฟเฟอร์ถูกตัดทอนเหลือ 32MB การเรนเดอร์อาจเสียหาย\n",
        MSG_AVISO "[ ⚠️ คำเตือน 203 ]" RESET " โหลดคีย์ที่กำหนดเองไม่สำเร็จ ใช้คีย์ในตัว\n",
        MSG_CMD_DIM "[ ⚙️ ดีบัก 214 ]" RESET " ตรวจพบความคลาดเคลื่อนทางเรขาคณิต การรักษาเสถียรภาพอาจแตกต่างกันไป\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    },

    // ---------------- [18] KHMER (KM) ----------------
    {
        MSG_ERRO "កំហុសក្នុងការបើកឯកសារ\n" RESET,
        "អ្នកបង្កើតដើម: ",
        "ចងក្រងដោយ: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "បានកែប្រែ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - កម្មវិធីកែលម្អ Terminal កម្រិតខ្ពស់" RESET "\n",
        "ការប្រើប្រាស់: " BG_FOSCO " cat ឯកសារ | neonx [ជម្រើស] " RESET "\n\n"
        "  -m [0-11]          កំណត់រចនាប័ទ្មចលនា\n"
        "  -s [តម្លៃ]            ល្បឿនផ្លាស់ប្តូរ " MSG_CMD_DIM "(លំនាំដើម: 0.2)" RESET "\n"
        "  -f [តម្លៃ]            ប្រេកង់រលក " MSG_CMD_DIM "(លំនាំដើម: 0.3)" RESET "\n"
        "  -d [តម្លៃ]            រយៈពេលគិតជាវិនាទី " MSG_CMD_DIM "(0 = គ្មានដែនកំណត់)" RESET "\n"
        "  -max-lines [តម្លៃ]    ដែនកំណត់បន្ទាត់អតិបរមា " MSG_CMD_DIM "(លំនាំដើម: 10.000)" RESET "\n"
        "  -A [ដឺក្រេ]           បង្វិលមុំនៃការไล่ระดับពណ៌ " MSG_CMD_DIM "(0-360)" RESET "\n"
        "  -p, -P [តម្លៃ]        កំណត់ Seed ថេរ " MSG_CMD_DIM "(កំណត់ទុកជាមុន)" RESET "\n"
        "  -S                 បង្ហាញស៊ុមឋិតិវន្ត " MSG_CMD_DIM "(គ្មានចលនា)" RESET "\n"
        "  -c [ទទឹង]           បង្ខំទទឹងឋិតិវន្តសម្រាប់ការไล่ระดับពណ៌\n"
        "  -o [0-1]           កែសម្រួលភាពថ្លា/ភាពរលោងផ្ដេក\n"
        "  -O [0-1]           បើកភាពថ្លាបញ្ឈរ (បន្ថយពណ៌លើ/ក្រោម)\n"
        "  -F [តម្លៃ]            ចាក់សោអត្រាស៊ុម " MSG_CMD_DIM "(ឧ: 60, 90)" RESET "\n"
        "  -L                 ដំណើរការមួយបន្ទាត់ម្ដងៗ " MSG_CMD_DIM "(ស្ទ្រីម)" RESET "\n"
        "  --fo [0-1]         បើកមុខងារ Matte (កាត់បន្ថយភាពស្រស់ឆើតឆាយ)\n"
        "  --preset [ឈ្មោះ]     ផ្ទុកក្ដារលាយពណ៌ " MSG_CMD_DIM "(cyberpunk, retro, matrix...)" RESET "\n"
        " - -c1, --c2 [hex]   ឈ្មោះក្លែងក្លាយសម្រាប់ --color1 និង --color2\n"
        "  --color1 [hex]     កំណត់ពណ៌ចាប់ផ្ដើមនៃការไล่ระดับពណ៌ (ឧ: #FF0000)\n"
        "  --color2 [hex]     កំណត់ពណ៌បញ្ចប់នៃការไล่ระดับពណ៌ (ឧ: #FFA500)\n"
        "  --quantized        ការធ្វើបរិមាណពណ៌ " MSG_CMD_DIM "(ដំណើរការលឿនជាងមុន)" RESET "\n"
        "  --no-ansi          បិទពណ៌ ANSI នៅក្នុងលទ្ធផល\n"
        "  --spin             បង្កើតកូដ ANSI សុទ្ធ " MSG_CMD_DIM "(សម្រាប់ស្គ្រីប)" RESET "\n"
        "  --lang [id]        បដិសេធភាសា " MSG_CMD_DIM "(ឧ: km, en)" RESET "\n"
        "  --license          បង្ហាញលក្ខខណ្ឌអាជ្ញាប័ណ្ណ\n"
        "  -v, --version      បង្ហាញកំណែ និងស្ថានភាពរបស់ប្រព័ន្ធគោលពីរ\n"
        "  -h, --help         បង្ហាញផ្ទាំងជំនួយអន្តរកម្មនេះ\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " ជម្រើស '%s' ទាមទារតម្លៃលេខបន្ទាប់ពីវា។\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " ជម្រើស '%s' ទាមទារលេខ។ បានទទួល: '%s'\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " របៀបចលនា (-m) ត្រូវតែជាចំនួនគត់។\n",
        MSG_INFO "[ ℹ️ ព័ត៌មាន 416 ]" RESET " របៀបចលនា (-m) ត្រូវតែចន្លោះពី 0 ដល់ 11។\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " ជម្រើសមិនត្រឹមត្រូវ ឬអាគុយម៉ង់រលុង '%s'\n",
        "\n" MSG_ERRO "[ ❌ កំហុស 404 ]" RESET " គ្មានទិន្នន័យត្រូវបានបញ្ជូនទៅគោលពីរទេ!\n",
        "\n" MSG_ERRO "[ ❌ កំហុស 413 ]" RESET " ឯកសារធំពេក។ ប្រើ -L សម្រាប់របៀបស្ទ្រីម។\n",
        MSG_AVISO "[ ⚠️ ព្រមាន 403 ]" RESET " មិនអាចផ្ទៀងផ្ទាត់សុចរិតភាពបានទេ (ប្រព័ន្ធដែលបានដាក់កម្រិត)។\n",
        MSG_ERRO "[ ❌ កំហុស 403 ]" RESET " បរាជ័យក្នុងការបើកឯកសារដែលអាចប្រតិបត្តិបានសម្រាប់ការផ្ទៀងផ្ទាត់។\n",
        MSG_ERRO "[ ❌ កំហុស 403 ]" RESET " កំហុសក្នុងការអានឯកសារកំឡុងពេលផ្ទៀងផ្ទាត់សុចរិតភាព។\n",
        MSG_ERRO "[ ❌ កំហុស 403 ]" RESET " អង្គចងចាំមិនគ្រប់គ្រាន់ដើម្បីផ្ទៀងផ្ទាត់សុចរិតភាព។\n",
        MSG_ERRO "[ ❌ កំហុស 403 ]" RESET " ហត្ថលេខាមិនត្រឹមត្រូវ ឬទម្រង់គោលដប់ប្រាំមួយមិនត្រឹមត្រូវ។\n",
        MSG_ERRO "[ ❌ កំហុស 403 ]" RESET " ឯកសារតូចពេកមិនអាចផ្ទុកហត្ថលេខាបានទេ។\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " ជម្រើស '%s' ទាមទារចំនួនគត់។\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " ជម្រើស '%s' ទាមទារតម្លៃវិជ្ជមាន។\n",
        MSG_ERRO "[ ❌ កំហុស 400 ]" RESET " រយៈពេលមិនអាចអវិជ្ជមានបានទេ។\n",
        MSG_ERRO "[ ❌ កំហុស 206 ]" RESET " មិនអាចបែងចែកអង្គចងចាំបានទេ (wcsdup)។\n",
        MSG_AVISO "[ ⚠️ ព្រមាន 403 ]" RESET " សតិបណ្ដោះអាសន្នត្រូវបានកាត់ឱ្យខ្លីត្រឹម 32MB។ ការបង្ហាញអាចខូច។\n",
        MSG_AVISO "[ ⚠️ ព្រមាន 203 ]" RESET " បរាជ័យក្នុងការផ្ទុកសោផ្ទាល់ខ្លួន ដោយប្រើសោដែលភ្ជាប់មកជាមួយ។\n",
        MSG_CMD_DIM "[ ⚙️ ឌីបាក 214 ]" RESET " រកឃើញការមិនតម្រឹមធរណីមាត្រ។ ការរក្សាលំនឹងអាចប្រែប្រួល។\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
    }
};

uint32_t voffset(void) {
    uint32_t c1 = NX_PHASE_STEP ^ 0x55555555UL;
    uint32_t c2 = 0x55555555UL;
    return c1 ^ c2;
}

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
