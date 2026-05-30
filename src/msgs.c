#include "msgs.h"
#include "style.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

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
// =========================================================================
// MACROS DE LICENÇA (Com os novos estilos ANSI)
// =========================================================================
#define LICENSE_PT \
    "LICENÇA DE USO - NEONX (C - VERSION)\n" \
    "-----------------------------------------------------------------\n" \
    "Copyright (c) 2026 @inrryoff - Licenciado sob condições especiais " LOGO_NEONX " LICENSE\n\n" \
    "Pelo presente, fica concedida permissão a qualquer pessoa que obtenha uma cópia\n" \
    "deste software para usá-lo gratuitamente, sujeito às seguintes condições:\n\n" \
    MSG_LEGAL_TXT "1. ATRIBUIÇÃO (CRÉDITOS):" RESET "\n" \
    "   O nome do autor original (@inrryoff) and os avisos de copyright devem ser\n" \
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
    "Copyright (c) 2026 @inrryoff - Licensed under special conditions " LOGO_NEONX " LICENSE\n\n" \
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n" \
    "of this software and associated documentation files, subject to the following:\n\n" \
    MSG_LEGAL_TXT "1. ATTRIBUTION (CREDITS):" RESET "\n" \
    "   The original author's name (@inrryoff) and copyright notices must be\n" \
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
// MATRIZ INTERNACIONAL (13 Idiomas)
// Índices: 0:PT, 1:EN, 2:ES, 3:FR, 4:DE, 5:IT, 6:RU, 7:ZH, 8:JA, 9:KO, 10:TR, 11:PL, 12:ID
// =========================================================================
static const char *mensagens[19][MSG_TOTAL] = {
    // ---------------- [0] PORTUGUÊS (PT) ----------------
    {
        MSG_ERRO "Erro ao abrir arquivo\n" RESET,
        "Criador Original: ", "Compilado por: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFICADO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_PT,
        LOGO_NEONX DIM ITALIC " - Embelezador de Terminal Avançado" RESET "\n",
        "Uso: " BG_FOSCO " cat arquivo | neonx [opcoes] " RESET "\n\n",
        "  -m [0-11]          Define o estilo da animação\n",
        "  -s [valor]         Velocidade da transição " MSG_CMD_DIM "(Padrão: 0.2)" RESET "\n",
        "  -f [valor]         Frequência da onda " MSG_CMD_DIM "(Padrão: 0.3)" RESET "\n",
        "  -d [valor]         Duração em segundos " MSG_CMD_DIM "(0 = Infinito)" RESET "\n",
        "  -A [graus]         Rotaciona o ângulo do gradiente " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [valor]         Define uma seed fixa " MSG_CMD_DIM "(Determinística)" RESET "\n",
        "  -S                 Renderiza um quadro estático " MSG_CMD_DIM "(Sem animação)" RESET "\n",
        "  -c [largura]       Força uma largura estática para o gradiente\n",
        "  -o [0-1]           Ajusta a opacidade/suavidade das bordas\n",
        "  -F [valor]         Trava a taxa de quadros " MSG_CMD_DIM "(ex: 60, 90)" RESET "\n",
        "  -L                 Processamento linha por linha " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [nome]    Carrega paletas " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Define a cor inicial do gradiente (ex: #FF0000)\n",
        "  --color2 [hex]     Define a cor final do gradiente (ex: #FFA500)\n",
        "  --quantized        Quantização de cores " MSG_CMD_DIM "(Maior performance)" RESET "\n",
        "  --spin             Gera códigos ANSI puros " MSG_CMD_DIM "(Para scripts)" RESET "\n",
        "  --lang [id]        Sobrescreve o idioma " MSG_CMD_DIM "(ex: pt, en)" RESET "\n",
        "  --license          Exibe os termos de licenciamento\n",
        "  -v, --version      Mostra a versão e status do binário\n",
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
        MSG_AVISO "[ ⚠️ AVISO 403 ]" RESET " Buffer truncado para 32MB. Renderização pode falhar.\n",
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ AVISO ]" RESET " Falha ao carregar chave customizada, usando embutida.\n"
    },

    // ---------------- [1] INGLÊS (EN) ----------------
    {
        MSG_ERRO "Error opening file\n" RESET,
        "Original Creator: ", "Compiled by: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFIED" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Advanced Terminal Beautifier" RESET "\n",
        "Usage: " BG_FOSCO " cat file | neonx [options] " RESET "\n\n",
        "  -m [0-11]          Defines the animation style\n",
        "  -s [val]           Transition speed " MSG_CMD_DIM "(Default: 0.2)" RESET "\n",
        "  -f [val]           Wave frequency " MSG_CMD_DIM "(Default: 0.3)" RESET "\n",
        "  -d [val]           Duration in seconds " MSG_CMD_DIM "(0 = Infinite)" RESET "\n",
        "  -A [deg]           Rotates the gradient angle " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [val]           Sets a fixed seed " MSG_CMD_DIM "(Deterministic)" RESET "\n",
        "  -S                 Renders a static frame " MSG_CMD_DIM "(No animation)" RESET "\n",
        "  -c [width]         Forces a static gradient width\n",
        "  -o [0-1]           Adjusts edge opacity/smoothness\n",
        "  -F [val]           Locks the framerate " MSG_CMD_DIM "(e.g., 60, 90)" RESET "\n",
        "  -L                 Line-by-line processing " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [name]    Loads color palettes " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Color quantization " MSG_CMD_DIM "(Higher performance)" RESET "\n",
        "  --spin             Generates pure ANSI codes " MSG_CMD_DIM "(For scripts)" RESET "\n",
        "  --lang [id]        Overrides interface language " MSG_CMD_DIM "(e.g., pt, en)" RESET "\n",
        "  --license          Displays software licensing terms\n",
        "  -v, --version      Shows binary version and build status\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ WARN ]" RESET " Failed to load custom key, using built-in key.\n"
    },

    // ---------------- [2] ESPANHOL (ES) ----------------
    {
        MSG_ERRO "Error al abrir el archivo\n" RESET,
        "Creador Original: ", "Compilado por: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFICADO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Embellecedor de Terminal Avanzado" RESET "\n",
        "Uso: " BG_FOSCO " cat archivo | neonx [opciones] " RESET "\n\n",
        "  -m [0-11]          Define el estilo de animación\n",
        "  -s [val]           Velocidad de transición " MSG_CMD_DIM "(Por defecto: 0.2)" RESET "\n",
        "  -f [val]           Frecuencia de onda " MSG_CMD_DIM "(Por defecto: 0.3)" RESET "\n",
        "  -d [val]           Duración en segundos " MSG_CMD_DIM "(0 = Infinito)" RESET "\n",
        "  -A [grados]        Rota el ángulo del gradiente " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [val]           Establece semilla fija " MSG_CMD_DIM "(Determinista)" RESET "\n",
        "  -S                 Renderiza un cuadro estático " MSG_CMD_DIM "(Sin animación)" RESET "\n",
        "  -c [ancho]         Fuerza un ancho de gradiente estático\n",
        "  -o [0-1]           Ajusta la opacidad/suavidad de los bordes\n",
        "  -F [val]           Bloquea los cuadros por segundo " MSG_CMD_DIM "(ej: 60, 90)" RESET "\n",
        "  -L                 Procesamiento línea por línea " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [nom]     Carga paletas de colores " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Cuantización de colores " MSG_CMD_DIM "(Mayor rendimiento)" RESET "\n",
        "  --spin             Genera códigos ANSI puros " MSG_CMD_DIM "(Para scripts)" RESET "\n",
        "  --lang [id]        Sobrescribe el idioma " MSG_CMD_DIM "(ej: pt, en)" RESET "\n",
        "  --license          Muestra los términos de licencia\n",
        "  -v, --version      Muestra la versión y estado del binario\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ AVISO ]" RESET " Fallo al cargar clave personalizada, usando la integrada.\n"
    },

    // ---------------- [3] FRANCÊS (FR) ----------------
    {
        MSG_ERRO "Erreur d'ouverture du fichier\n" RESET,
        "Créateur Original : ", "Compilé par : ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFIÉ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Embellisseur de Terminal Avancé" RESET "\n",
        "Utilisation: " BG_FOSCO " cat fichier | neonx [options] " RESET "\n\n",
        "  -m [0-11]          Définit le style d'animation\n",
        "  -s [val]           Vitesse de transition " MSG_CMD_DIM "(Défaut: 0.2)" RESET "\n",
        "  -f [val]           Fréquence de l'onde " MSG_CMD_DIM "(Défaut: 0.3)" RESET "\n",
        "  -d [val]           Durée en secondes " MSG_CMD_DIM "(0 = Infini)" RESET "\n",
        "  -A [deg]           Fait pivoter l'angle du dégradé " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [val]           Définit une graine fixe " MSG_CMD_DIM "(Déterministe)" RESET "\n",
        "  -S                 Affiche une image statique " MSG_CMD_DIM "(Sans animation)" RESET "\n",
        "  -c [largeur]       Force une largeur de dégradé statique\n",
        "  -o [0-1]           Ajuste l'opacité/douceur des bords\n",
        "  -F [val]           Verrouille le framerate " MSG_CMD_DIM "(ex: 60, 90)" RESET "\n",
        "  -L                 Traitement ligne par ligne " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [nom]     Charge des palettes " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Quantification des couleurs " MSG_CMD_DIM "(Hautes performances)" RESET "\n",
        "  --spin             Génère des codes ANSI purs " MSG_CMD_DIM "(Pour scripts)" RESET "\n",
        "  --lang [id]        Remplace la langue de l'interface " MSG_CMD_DIM "(ex: pt, en)" RESET "\n",
        "  --license          Affiche les conditions de licence\n",
        "  -v, --version      Affiche la version et l'état du binaire\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ AVERT ]" RESET " Échec du chargement de la clé, utilisation de celle par défaut.\n"
    },

    // ---------------- [4] ALEMÃO (DE) ----------------
    {
        MSG_ERRO "Fehler beim Öffnen der Datei\n" RESET,
        "Originalentwickler: ", "Kompiliert von: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFIZIERT" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Erweiterter Terminal-Verschönerer" RESET "\n",
        "Verwendung: " BG_FOSCO " cat datei | neonx [optionen] " RESET "\n\n",
        "  -m [0-11]          Legt den Animationsstil fest\n",
        "  -s [Wert]          Übergangsgeschwindigkeit " MSG_CMD_DIM "(Standard: 0.2)" RESET "\n",
        "  -f [Wert]          Wellenfrequenz " MSG_CMD_DIM "(Standard: 0.3)" RESET "\n",
        "  -d [Wert]          Dauer in Sekunden " MSG_CMD_DIM "(0 = Unendlich)" RESET "\n",
        "  -A [Grad]          Dreht den Gradientenwinkel " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [Wert]          Legt festen Seed fest " MSG_CMD_DIM "(Deterministisch)" RESET "\n",
        "  -S                 Rendert ein statisches Bild " MSG_CMD_DIM "(Keine Animation)" RESET "\n",
        "  -c [Breite]        Erzwingt statische Gradientenbreite\n",
        "  -o [0-1]         Passt die Randopazität/Glätte an\n",
        "  -F [Wert]          Sperrt die Bildrate " MSG_CMD_DIM "(z.B. 60, 90)" RESET "\n",
        "  -L                 Zeilenweise Verarbeitung " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [Name]    Lädt Farbpaletten " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Farbquantisierung " MSG_CMD_DIM "(Höhere Leistung)" RESET "\n",
        "  --spin             Generiert reine ANSI-Codes " MSG_CMD_DIM "(Für Skripte)" RESET "\n",
        "  --lang [id]        Überschreibt die Sprache " MSG_CMD_DIM "(z.B. pt, en)" RESET "\n",
        "  --license          Zeigt Lizenzbedingungen an\n",
        "  -v, --version      Zeigt Binärversion und Build-Status\n",
        "  -h, --help         Zeigt dieses interaktive Hilfepanel\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ WARNUNG ]" RESET " Benutzerdefinierter Schlüssel fehlgeschlagen, verwende internen.\n"
    },

    // ---------------- [5] ITALIANO (IT) ----------------
    {
        MSG_ERRO "Errore durante l'apertura del file\n" RESET,
        "Creatore Originale: ", "Compilato da: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "MODIFICATO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Abbellitore di Terminale Avanzato" RESET "\n",
        "Uso: " BG_FOSCO " cat file | neonx [opzioni] " RESET "\n\n",
        "  -m [0-11]          Definisce lo stile dell'animazione\n",
        "  -s [val]           Velocità di transizione " MSG_CMD_DIM "(Predefinito: 0.2)" RESET "\n",
        "  -f [val]           Frequenza dell'onda " MSG_CMD_DIM "(Predefinito: 0.3)" RESET "\n",
        "  -d [val]           Durata in secondi " MSG_CMD_DIM "(0 = Infinito)" RESET "\n",
        "  -A [gradi]         Ruota l'angolo del gradiente " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [val]           Imposta un seed fisso " MSG_CMD_DIM "(Deterministico)" RESET "\n",
        "  -S                 Mostra un frame statico " MSG_CMD_DIM "(Senza animazione)" RESET "\n",
        "  -c [larg]          Forza una larghezza statica del gradiente\n",
        "  -o [0-1]           Regola l'opacità/morbidezza dei bordi\n",
        "  -F [val]           Blocca il framerate " MSG_CMD_DIM "(es: 60, 90)" RESET "\n",
        "  -L                 Elaborazione riga per riga " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [nome]    Carica le tavolozze " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Quantizzazione colore " MSG_CMD_DIM "(Maggiori prestazioni)" RESET "\n",
        "  --spin             Genera codici ANSI puri " MSG_CMD_DIM "(Per script)" RESET "\n",
        "  --lang [id]        Sovrascrive la lingua " MSG_CMD_DIM "(es: pt, en)" RESET "\n",
        "  --license          Mostra i termini di licenza del software\n",
        "  -v, --version      Mostra versione binaria e stato della build\n",
        "  -h, --help         Mostra questo pannello di aiuto\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ AVVISO ]" RESET " Impossibile caricare la chiave, utilizzo quella integrata.\n"
    },

    // ---------------- [6] RUSSO (RU) ----------------
    {
        MSG_ERRO "Ошибка при открытии файла\n" RESET,
        "Оригинальный создатель: ", "Скомпилировано: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ИЗМЕНЕНО" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Расширенный украшатель терминала" RESET "\n",
        "Использование: " BG_FOSCO " cat файл | neonx [опции] " RESET "\n\n",
        "  -m [0-11]          Определяет стиль анимации\n",
        "  -s [знач]          Скорость перехода " MSG_CMD_DIM "(По умолч: 0.2)" RESET "\n",
        "  -f [знач]          Частота волны " MSG_CMD_DIM "(По умолч: 0.3)" RESET "\n",
        "  -d [знач]          Продолжительность в секундах " MSG_CMD_DIM "(0 = Бесконечно)" RESET "\n",
        "  -A [угол]          Поворачивает угол градиента " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [знач]          Устанавливает фиксированный сид " MSG_CMD_DIM "(Детерминировано)" RESET "\n",
        "  -S                 Рендерит статический кадр " MSG_CMD_DIM "(Без анимации)" RESET "\n",
        "  -c [ширина]        Принудительная ширина градиента\n",
        "  -o [0-1]           Настройка непрозрачности/гладкости краев\n",
        "  -F [знач]          Блокирует частоту кадров " MSG_CMD_DIM "(напр: 60, 90)" RESET "\n",
        "  -L                 Построчная обработка " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [имя]     Загружает цветовые палитры " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Квантование цвета " MSG_CMD_DIM "(Высокая производительность)" RESET "\n",
        "  --spin             Генерирует чистые коды ANSI " MSG_CMD_DIM "(Для скриптов)" RESET "\n",
        "  --lang [id]        Переопределяет язык " MSG_CMD_DIM "(напр: pt, en)" RESET "\n",
        "  --license          Показывает условия лицензирования\n",
        "  -v, --version      Показывает версию и статус сборки\n",
        "  -h, --help         Показывает эту панель помощи\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ ПРЕДУПР ]" RESET " Ошибка загрузки ключа, используется встроенный.\n"
    },

    // ---------------- [7] CHINÊS (ZH) ----------------
    {
        MSG_ERRO "打开文件时出错\n" RESET,
        "原作者: ", "编译者: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "已修改" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - 高级终端美化器" RESET "\n",
        "用法: " BG_FOSCO " cat 文件 | neonx [选项] " RESET "\n\n",
        "  -m [0-11]          定义动画样式\n",
        "  -s [值]            过渡速度 " MSG_CMD_DIM "(默认: 0.2)" RESET "\n",
        "  -f [值]            波浪频率 " MSG_CMD_DIM "(默认: 0.3)" RESET "\n",
        "  -d [值]            持续时间(秒) " MSG_CMD_DIM "(0 = 无限)" RESET "\n",
        "  -A [角度]          旋转渐变角度 " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [值]            设置固定种子 " MSG_CMD_DIM "(确定性生成)" RESET "\n",
        "  -S                 渲染静态帧 " MSG_CMD_DIM "(无动画)" RESET "\n",
        "  -c [宽度]          强制静态渐变宽度\n",
        "  -o [0-1]           调整边缘不透明度/平滑度\n",
        "  -F [值]            锁定帧率 " MSG_CMD_DIM "(例如: 60, 90)" RESET "\n",
        "  -L                 逐行处理模式 " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [名称]    加载调色板 " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        颜色量化模式 " MSG_CMD_DIM "(更高性能)" RESET "\n",
        "  --spin             生成纯 ANSI 代码 " MSG_CMD_DIM "(用于外部脚本)" RESET "\n",
        "  --lang [id]        覆盖界面语言 " MSG_CMD_DIM "(例如: pt, en)" RESET "\n",
        "  --license          显示软件许可条款\n",
        "  -v, --version      显示二进制版本和构建状态\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ 警告 ]" RESET " 加载自定义密钥失败，使用内置密钥。\n"
    },

    // ---------------- [8] JAPONÊS (JA) ----------------
    {
        MSG_ERRO "ファイルを開く際にエラーが発生しました\n" RESET,
        "オリジナルの作成者: ", "コンパイル: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "変更済み" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - 高度なターミナル装飾ツール" RESET "\n",
        "使用方法: " BG_FOSCO " cat ファイル | neonx [オプション] " RESET "\n\n",
        "  -m [0-11]          アニメーションスタイルを定義\n",
        "  -s [値]            トランジション速度 " MSG_CMD_DIM "(デフォルト: 0.2)" RESET "\n",
        "  -f [値]            波の周波数 " MSG_CMD_DIM "(デフォルト: 0.3)" RESET "\n",
        "  -d [値]            継続時間(秒) " MSG_CMD_DIM "(0 = 無限)" RESET "\n",
        "  -A [角度]          グラデーションの角度を回転 " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [値]            固定シードを設定 " MSG_CMD_DIM "(決定論的)" RESET "\n",
        "  -S                 静的フレームをレンダリング " MSG_CMD_DIM "(アニメーションなし)" RESET "\n",
        "  -c [幅]            静的グラデーションの幅を強制\n",
        "  -o [0-1]           エッジの不透明度/滑らかさを調整\n",
        "  -F [値]            フレームレートをロック " MSG_CMD_DIM "(例: 60, 90)" RESET "\n",
        "  -L                 行ごとの処理 " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [名前]    カラーパレットをロード " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        カラー量子化 " MSG_CMD_DIM "(高パフォーマンス)" RESET "\n",
        "  --spin             純粋な ANSI コードを生成 " MSG_CMD_DIM "(スクリプト用)" RESET "\n",
        "  --lang [id]        言語を上書き " MSG_CMD_DIM "(例: pt, en)" RESET "\n",
        "  --license          ソフトウェアのライセンス条項を表示\n",
        "  -v, --version      バイナリのバージョンとビルド状態を表示\n",
        "  -h, --help         このインタラクティブなヘルプを表示\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ 警告 ]" RESET " カスタムキーの読み込みに失敗しました。組み込みキーを使用します。\n"
    },

    // ---------------- [9] COREANO (KO) ----------------
    {
        MSG_ERRO "파일 열기 오류\n" RESET,
        "원본 제작자: ", "컴파일러: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "수정됨" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - 고급 터미널 뷰티파이어" RESET "\n",
        "사용법: " BG_FOSCO " cat 파일 | neonx [옵션] " RESET "\n\n",
        "  -m [0-11]          애니메이션 스타일 정의\n",
        "  -s [값]            전환 속도 " MSG_CMD_DIM "(기본값: 0.2)" RESET "\n",
        "  -f [값]            파동 주파수 " MSG_CMD_DIM "(기본값: 0.3)" RESET "\n",
        "  -d [값]            지속 시간(초) " MSG_CMD_DIM "(0 = 무한)" RESET "\n",
        "  -A [각도]          그라디언트 각도 회전 " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [값]            고정 시드 설정 " MSG_CMD_DIM "(결정론적)" RESET "\n",
        "  -S                 정적 프레임 렌더링 " MSG_CMD_DIM "(애니메이션 없음)" RESET "\n",
        "  -c [너비]          정적 그라디언트 너비 강제 적용\n",
        "  -o [0-1]           가장자리 불투명도/부드러움 조정\n",
        "  -F [값]            프레임 속도 고정 " MSG_CMD_DIM "(예: 60, 90)" RESET "\n",
        "  -L                 한 줄씩 처리 " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [이름]    색상 팔레트 로드 " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        색상 양자화 " MSG_CMD_DIM "(고성능 모드)" RESET "\n",
        "  --spin             순수 ANSI 코드 생성 " MSG_CMD_DIM "(스크립트용)" RESET "\n",
        "  --lang [id]        인터페이스 언어 재정의 " MSG_CMD_DIM "(예: pt, en)" RESET "\n",
        "  --license          소프트웨어 라이선스 조항 표시\n",
        "  -v, --version      바이너리 버전 및 빌드 상태 표시\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ 경고 ]" RESET " 사용자 지정 키를 로드하지 못했습니다. 기본 키를 사용합니다.\n"
    },

    // ---------------- [10] TURCO (TR) ----------------
    {
        MSG_ERRO "Dosya açılırken hata oluştu\n" RESET,
        "Orijinal Yapımcı: ", "Derleyen: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "DEĞİŞTİRİLDİ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Gelişmiş Terminal Güzelleştirici" RESET "\n",
        "Kullanım: " BG_FOSCO " cat dosya | neonx [seçenekler] " RESET "\n\n",
        "  -m [0-11]          Animasyon stilini tanımlar\n",
        "  -s [değer]         Geçiş hızı " MSG_CMD_DIM "(Varsayılan: 0.2)" RESET "\n",
        "  -f [değer]         Dalga frekansı " MSG_CMD_DIM "(Varsayılan: 0.3)" RESET "\n",
        "  -d [değer]         Saniye cinsinden süre " MSG_CMD_DIM "(0 = Sonsuz)" RESET "\n",
        "  -A [açı]           Gradyan açısını döndürür " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [değer]         Sabit bir seed ayarlar " MSG_CMD_DIM "(Deterministik)" RESET "\n",
        "  -S                 Statik bir kare oluşturur " MSG_CMD_DIM "(Animasyon yok)" RESET "\n",
        "  -c [genişlik]      Statik gradyan genişliğine zorlar\n",
        "  -o [0-1]           Kenar opaklığını/pürüzsüzlüğünü ayarlar\n",
        "  -F [değer]         Kare hızını kilitler " MSG_CMD_DIM "(ör: 60, 90)" RESET "\n",
        "  -L                 Satır satır işleme " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [isim]    Renk paletlerini yükler " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Renk kuantizasyonu " MSG_CMD_DIM "(Daha yüksek performans)" RESET "\n",
        "  --spin             Saf ANSI kodları üretir " MSG_CMD_DIM "(Scriptler için)" RESET "\n",
        "  --lang [id]        Arayüz dilini geçersiz kılar " MSG_CMD_DIM "(ör: pt, en)" RESET "\n",
        "  --license          Yazılım lisans koşullarını gösterir\n",
        "  -v, --version      İkili sürümü ve derleme durumunu gösterir\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ UYARI ]" RESET " Özel anahtar yüklenemedi, yerleşik anahtar kullanılıyor.\n"
    },

    // ---------------- [11] POLONÊS (PL) ----------------
    {
        MSG_ERRO "Błąd podczas otwierania pliku\n" RESET,
        "Oryginalny Twórca: ", "Skompilowane przez: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ZMODYFIKOWANO" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Zaawansowany Upiększacz Terminala" RESET "\n",
        "Użycie: " BG_FOSCO " cat plik | neonx [opcje] " RESET "\n\n",
        "  -m [0-11]          Definiuje styl animacji\n",
        "  -s [wartość]       Prędkość przejścia " MSG_CMD_DIM "(Domyślnie: 0.2)" RESET "\n",
        "  -f [wartość]       Częstotliwość fali " MSG_CMD_DIM "(Domyślnie: 0.3)" RESET "\n",
        "  -d [wartość]       Czas w sekundach " MSG_CMD_DIM "(0 = Nieskończoność)" RESET "\n",
        "  -A [kąt]           Obraca kąt gradientu " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [wartość]       Ustawia stały seed " MSG_CMD_DIM "(Deterministyczny)" RESET "\n",
        "  -S                 Generuje statyczną klatkę " MSG_CMD_DIM "(Brak animacji)" RESET "\n",
        "  -c [szerokość]     Wymusza stałą szerokość gradientu\n",
        "  -o [0-1]           Dostosowuje krycie/gładkość krawędzi\n",
        "  -F [wartość]       Blokuje liczbę klatek " MSG_CMD_DIM "(np. 60, 90)" RESET "\n",
        "  -L                 Przetwarzanie linia po linii " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [nazwa]   Ładuje palety kolorów " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Kwantyzacja kolorów " MSG_CMD_DIM "(Wyższa wydajność)" RESET "\n",
        "  --spin             Generuje czyste kody ANSI " MSG_CMD_DIM "(Dla skryptów)" RESET "\n",
        "  --lang [id]        Nadpisuje język interfejsu " MSG_CMD_DIM "(np. pt, en)" RESET "\n",
        "  --license          Wyświetla warunki licencji oprogramowania\n",
        "  -v, --version      Pokazuje wersję binarną i status kompilacji\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ OSTRZ ]" RESET " Błąd ładowania własnego klucza, używam wbudowanego.\n"
    },

    // ---------------- [12] INDONÉSIO (ID) ----------------
    {
        MSG_ERRO "Kesalahan saat membuka file\n" RESET,
        "Pembuat Asli: ", "Dikompalasi oleh: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "DIMODIFIKASI" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Penata Terminal Lanjutan" RESET "\n",
        "Penggunaan: " BG_FOSCO " cat file | neonx [opsi] " RESET "\n\n",
        "  -m [0-11]          Menentukan gaya animasi\n",
        "  -s [nilai]         Kecepatan transisi " MSG_CMD_DIM "(Default: 0.2)" RESET "\n",
        "  -f [nilai]         Frekuensi gelombang " MSG_CMD_DIM "(Default: 0.3)" RESET "\n",
        "  -d [nilai]         Durasi dalam detik " MSG_CMD_DIM "(0 = Tak terbatas)" RESET "\n",
        "  -A [sudut]         Memutar sudut gradien " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [nilai]         Menetapkan seed tetap " MSG_CMD_DIM "(Deterministik)" RESET "\n",
        "  -S                 Merender bingkai statis " MSG_CMD_DIM "(Tanpa animasi)" RESET "\n",
        "  -c [lebar]         Memaksa lebar gradien statis\n",
        "  -o [0-1]           Menyesuaikan opasitas/kehalusan tepi\n",
        "  -F [nilai]         Mengunci kecepatan bingkai " MSG_CMD_DIM "(mis: 60, 90)" RESET "\n",
        "  -L                 Pemrosesan baris demi baris " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [nama]    Memuat palet warna " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Kuantisasi warna " MSG_CMD_DIM "(Performa lebih tinggi)" RESET "\n",
        "  --spin             Menghasilkan kode ANSI murni " MSG_CMD_DIM "(Untuk skrip)" RESET "\n",
        "  --lang [id]        Menimpa bahasa antarmuka " MSG_CMD_DIM "(mis: pt, en)" RESET "\n",
        "  --license          Menampilkan persyaratan lisensi\n",
        "  -v, --version      Menampilkan versi biner dan status build\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ AWAS ]" RESET " Gagal memuat kunci kustom, menggunakan kunci bawaan.\n"
    },

    // ---------------- [13] ÁRABE (AR) ----------------
    {
        MSG_ERRO "خطأ في فتح الملف\n" RESET,
        "المبتكر الأصلي: ", "تم التجميع بواسطة: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "تم التعديل" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - مُحسّن الطرفية المتقدم" RESET "\n",
        "الاستخدام: " BG_FOSCO " cat ملف | neonx [خيارات] " RESET "\n\n",
        "  -m [0-11]          يحدد نمط الرسوم المتحركة\n",
        "  -s [قيمة]          سرعة الانتقال " MSG_CMD_DIM "(الافتراضي: 0.2)" RESET "\n",
        "  -f [قيمة]          تردد الموجة " MSG_CMD_DIM "(الافتراضي: 0.3)" RESET "\n",
        "  -d [قيمة]          المدة بالثواني " MSG_CMD_DIM "(0 = لانهائي)" RESET "\n",
        "  -A [زاوية]         تدوير زاوية التدرج " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [قيمة]          يعين بذرة ثابتة " MSG_CMD_DIM "(حتمي)" RESET "\n",
        "  -S                 يعرض إطارًا ثابتًا " MSG_CMD_DIM "(بدون رسوم متحركة)" RESET "\n",
        "  -c [عرض]           يفرض عرض تدرج ثابت\n",
        "  -o [0-1]           يضبط شفافية/نعومة الحواف\n",
        "  -F [قيمة]          يُقفل معدل الإطارات " MSG_CMD_DIM "(مثل: 60, 90)" RESET "\n",
        "  -L                 معالجة سطر بسطر " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [اسم]     يحمل لوحات الألوان " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        تكميم اللون " MSG_CMD_DIM "(أداء أعلى)" RESET "\n",
        "  --spin             يولد أكواد ANSI نقية " MSG_CMD_DIM "(للنصوص البرمجية)" RESET "\n",
        "  --lang [id]        يتجاوز لغة الواجهة " MSG_CMD_DIM "(مثل: pt, en)" RESET "\n",
        "  --license          يعرض شروط ترخيص البرنامج\n",
        "  -v, --version      يعرض إصدار الثنائي وحالة البناء\n",
        "  -h, --help         يعرض لوحة المساعدة التفاعلية هذه\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ تحذير ]" RESET " فشل تحميل المفتاح المخصص، جاري استخدام المدمج.\n"
    },

    // ---------------- [14] BÚLGARO (BG) ----------------
    {
        MSG_ERRO "Грешка при отваряне на файл\n" RESET,
        "Оригинален създател: ", "Компилирано от: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "МОДИФИЦИРАН" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Разширен разкрасител на терминал" RESET "\n",
        "Употреба: " BG_FOSCO " cat файл | neonx [опции] " RESET "\n\n",
        "  -m [0-11]          Определя стила на анимацията\n",
        "  -s [стойност]      Скорост на прехода " MSG_CMD_DIM "(По подразбиране: 0.2)" RESET "\n",
        "  -f [стойност]      Честота на вълната " MSG_CMD_DIM "(По подразбиране: 0.3)" RESET "\n",
        "  -d [стойност]      Продължителност в секунди " MSG_CMD_DIM "(0 = Безкрайно)" RESET "\n",
        "  -A [ъгъл]          Завърта ъгъла на градиента " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [стойност]      Задава фиксиран сийд " MSG_CMD_DIM "(Детерминирано)" RESET "\n",
        "  -S                 Рендерира статичен кадър " MSG_CMD_DIM "(Без анимация)" RESET "\n",
        "  -c [ширина]        Принудителна статична ширина на градиента\n",
        "  -o [0-1]           Регулира непрозрачността/гладкостта на ръбовете\n",
        "  -F [стойност]      Заключва кадрите в секунда " MSG_CMD_DIM "(напр. 60, 90)" RESET "\n",
        "  -L                 Обработка ред по ред " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [име]     Зарежда цветови палитри " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Квантоване на цветовете " MSG_CMD_DIM "(По-висока производителност)" RESET "\n",
        "  --spin             Генерира чисти ANSI кодове " MSG_CMD_DIM "(За скриптове)" RESET "\n",
        "  --lang [id]        Презаписва езика на интерфейса " MSG_CMD_DIM "(напр. pt, en)" RESET "\n",
        "  --license          Показва условията за лицензиране\n",
        "  -v, --version      Показва версията и статуса на компилацията\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ ВНИМАНИЕ ]" RESET " Неуспешно зареждане на персонализиран ключ, използва се вграденият.\n"
    },

    // ---------------- [15] GREGO (EL) ----------------
    {
        MSG_ERRO "Σφάλμα κατά το άνοιγμα του αρχείου\n" RESET,
        "Αρχικός Δημιουργός: ", "Μεταγλωττίστηκε από: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ΤΡΟΠΟΠΟΙΗΜΕΝΟ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - Προηγμένο Εργαλείο Καλλωπισμού Τερματικού" RESET "\n",
        "Χρήση: " BG_FOSCO " cat αρχείο | neonx [επιλογές] " RESET "\n\n",
        "  -m [0-11]          Ορίζει το στυλ κιν. σχεδίων\n",
        "  -s [τιμή]          Ταχύτητα μετάβασης " MSG_CMD_DIM "(Προεπιλογή: 0.2)" RESET "\n",
        "  -f [τιμή]          Συχνότητα κύματος " MSG_CMD_DIM "(Προεπιλογή: 0.3)" RESET "\n",
        "  -d [τιμή]          Διάρκεια σε δευτερόλεπτα " MSG_CMD_DIM "(0 = Άπειρο)" RESET "\n",
        "  -A [μοίρες]        Περιστρέφει τη γωνία ντεγκραντέ " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [τιμή]          Ορίζει σταθερό σπόρο " MSG_CMD_DIM "(Ντετερμινιστικό)" RESET "\n",
        "  -S                 Εμφανίζει στατικό καρέ " MSG_CMD_DIM "(Χωρίς κίνηση)" RESET "\n",
        "  -c [πλάτος]        Επιβάλλει σταθερό πλάτος ντεγκραντέ\n",
        "  -o [0-1]           Ρυθμίζει την αδιαφάνεια/ομαλότητα των άκρων\n",
        "  -F [τιμή]          Κλειδώνει το framerate " MSG_CMD_DIM "(π.χ. 60, 90)" RESET "\n",
        "  -L                 Επεξεργασία γραμμή προς γραμμή " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [όνομα]   Φορτώνει παλέτες χρωμάτων " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        Κβαντισμός χρώματος " MSG_CMD_DIM "(Υψηλότερη απόδοση)" RESET "\n",
        "  --spin             Δημιουργεί καθαρούς κωδικούς ANSI " MSG_CMD_DIM "(Για scripts)" RESET "\n",
        "  --lang [id]        Παρακάμπτει τη γλώσσα " MSG_CMD_DIM "(π.χ. pt, en)" RESET "\n",
        "  --license          Εμφανίζει τους όρους άδειας χρήσης\n",
        "  -v, --version      Δείχνει την έκδοση και την κατάσταση του build\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ ΠΡΟΕΙΔ ]" RESET " Αποτυχία φόρτωσης προσαρμοσμένου κλειδιού, χρήση ενσωματωμένου.\n"
    },

    // ---------------- [16] HINDI (HI) ----------------
    {
        MSG_ERRO "फ़ाइल खोलने में त्रुटि\n" RESET,
        "मूल निर्माता: ", "द्वारा संकलित: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "संशोधित" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - उन्नत टर्मिनल ब्यूटीफायर" RESET "\n",
        "उपयोग: " BG_FOSCO " cat फ़ाइल | neonx [विकल्प] " RESET "\n\n",
        "  -m [0-11]          एनीमेशन शैली को परिभाषित करता है\n",
        "  -s [मान]           संक्रमण गति " MSG_CMD_DIM "(डिफ़ॉल्ट: 0.2)" RESET "\n",
        "  -f [मान]           तरंग आवृत्ति " MSG_CMD_DIM "(डिफ़ॉल्ट: 0.3)" RESET "\n",
        "  -d [मान]           सेकंड में अवधि " MSG_CMD_DIM "(0 = अनंत)" RESET "\n",
        "  -A [कोण]           ग्रेडिएंट कोण घुमाता है " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [मान]           स्थिर बीज सेट करता है " MSG_CMD_DIM "(निर्धारक)" RESET "\n",
        "  -S                 स्थिर फ्रेम प्रस्तुत करता है " MSG_CMD_DIM "(कोई एनीमेशन नहीं)" RESET "\n",
        "  -c [चौड़ाई]        स्थिर ग्रेडिएंट चौड़ाई लागू करता है\n",
        "  -o [0-1]           किनारे की अस्पष्टता/चिकनाई को समायोजित करता है\n",
        "  -F [मान]           फ्रेमरेट को लॉक करता है " MSG_CMD_DIM "(उदा. 60, 90)" RESET "\n",
        "  -L                 लाइन-बाय-लाइन प्रोसेसिंग " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [नाम]     रंग पट्टियाँ लोड करता है " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        रंग परिमाणीकरण " MSG_CMD_DIM "(उच्च प्रदर्शन)" RESET "\n",
        "  --spin             शुद्ध ANSI कोड उत्पन्न करता है " MSG_CMD_DIM "(स्क्रिप्ट के लिए)" RESET "\n",
        "  --lang [id]        इंटरफ़ेस भाषा को ओवरराइड करता है " MSG_CMD_DIM "(उदा. pt, en)" RESET "\n",
        "  --license          सॉफ़्टवेयर लाइसेंस शर्तें प्रदर्शित करता है\n",
        "  -v, --version      बाइनरी संस्करण और बिल्ड स्थिति दिखाता है\n",
        "  -h, --help         यह इंटरैक्टिव सहायता पैनल प्रदर्शित करता है\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ चेतावनी ]" RESET " कस्टम कुंजी लोड करने में विफल, अंतर्निहित का उपयोग कर रहा है।\n"
    },

    // ---------------- [17] TAILANDÊS (TH) ----------------
    {
        MSG_ERRO "ข้อผิดพลาดในการเปิดไฟล์\n" RESET,
        "ผู้สร้างดั้งเดิม: ", "คอมไพล์โดย: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "ถูกปรับเปลี่ยน" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - เครื่องมือตกแต่งเทอร์มินัลขั้นสูง" RESET "\n",
        "วิธีใช้: " BG_FOSCO " cat ไฟล์ | neonx [ตัวเลือก] " RESET "\n\n",
        "  -m [0-11]          กำหนดสไตล์แอนิเมชัน\n",
        "  -s [ค่า]            ความเร็วในการเปลี่ยนภาพ " MSG_CMD_DIM "(ค่าเริ่มต้น: 0.2)" RESET "\n",
        "  -f [ค่า]            ความถี่ของคลื่น " MSG_CMD_DIM "(ค่าเริ่มต้น: 0.3)" RESET "\n",
        "  -d [ค่า]            ระยะเวลาเป็นวินาที " MSG_CMD_DIM "(0 = อนันต์)" RESET "\n",
        "  -A [องศา]          หมุนมุมการไล่ระดับสี " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [ค่า]            ตั้งค่าซีดคงที่ " MSG_CMD_DIM "(แบบกำหนดได้)" RESET "\n",
        "  -S                 เรนเดอร์เฟรมคงที่ " MSG_CMD_DIM "(ไม่มีแอนิเมชัน)" RESET "\n",
        "  -c [ความกว้าง]     บังคับความกว้างการไล่ระดับสีแบบคงที่\n",
        "  -o [0-1]           ปรับความทึบ/ความเรียบเนียนของขอบ\n",
        "  -F [ค่า]            ล็อคอัตราเฟรม " MSG_CMD_DIM "(เช่น 60, 90)" RESET "\n",
        "  -L                 การประมวลผลทีละบรรทัด " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [ชื่อ]      โหลดจานสี " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        การลดทอนสี " MSG_CMD_DIM "(ประสิทธิภาพสูงขึ้น)" RESET "\n",
        "  --spin             สร้างรหัส ANSI ล้วน " MSG_CMD_DIM "(สำหรับสคริปต์)" RESET "\n",
        "  --lang [id]        แทนที่ภาษาอินเทอร์เฟซ " MSG_CMD_DIM "(เช่น pt, en)" RESET "\n",
        "  --license          แสดงเงื่อนไขการอนุญาตให้ใช้ซอฟต์แวร์\n",
        "  -v, --version      แสดงเวอร์ชันไบนารีและสถานะบิลด์\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ คำเตือน ]" RESET " โหลดคีย์ที่กำหนดเองไม่สำเร็จ ใช้คีย์ในตัว\n"
    },

    // ---------------- [18] KHMER (KM) ----------------
    {
        MSG_ERRO "កំហុសក្នុងការបើកឯកសារ\n" RESET,
        "អ្នកបង្កើតដើម: ", "ចងក្រងដោយ: ",
        "Status: " MSG_SUCESSO "%s" RESET "\n",
        "Status: " MSG_AVISO "%s" RESET "\n",
        "Status: " MSG_ERRO "បានកែប្រែ" RESET "\n",
        "Status: " MSG_AVISO "VERIFY_ERROR" RESET "\n",
        LICENSE_EN,
        LOGO_NEONX DIM ITALIC " - កម្មវិធីលម្អ Terminal កម្រិតខ្ពស់" RESET "\n",
        "ការប្រើប្រាស់: " BG_FOSCO " cat ឯកសារ | neonx [ជម្រើស] " RESET "\n\n",
        "  -m [0-11]          កំណត់រចនាប័ទ្មចលនា\n",
        "  -s [តម្លៃ]          ល្បឿនផ្លាស់ប្តូរ " MSG_CMD_DIM "(លំនាំដើម: 0.2)" RESET "\n",
        "  -f [តម្លៃ]          ប្រេកង់រលក " MSG_CMD_DIM "(លំនាំដើម: 0.3)" RESET "\n",
        "  -d [តម្លៃ]          រយៈពេលគិតជាវិនាទី " MSG_CMD_DIM "(0 = គ្មានដែនកំណត់)" RESET "\n",
        "  -A [ដឺក្រេ]         បង្វិលមុំជម្រាល " MSG_CMD_DIM "(0-360)" RESET "\n",
        "  -p [តម្លៃ]          កំណត់គ្រាប់ពូជថេរ " MSG_CMD_DIM "(កំណត់ទុកជាមុន)" RESET "\n",
        "  -S                 បង្ហាញស៊ុមឋិតិវន្ត " MSG_CMD_DIM "(គ្មានចលនា)" RESET "\n",
        "  -c [ទទឹង]          បង្ខំទទឹងជម្រាលថេរ\n",
        "  -o [0-1]           កែតម្រូវភាពស្រអាប់/ភាពរលោងនៃគែម\n",
        "  -F [តម្លៃ]          ចាក់សោអត្រាស៊ុម " MSG_CMD_DIM "(ឧ. 60, 90)" RESET "\n",
        "  -L                 ដំណើរការមួយបន្ទាត់ម្តង " MSG_CMD_DIM "(Stream)" RESET "\n",
        "  --preset [ឈ្មោះ]   ផ្ទុកក្ដារលាយពណ៌ " MSG_CMD_DIM "(cyberpunk, retro, matrix,\n"
        "                     sunset, vaporwave, ocean, forest, blood,\n"
        "                     hacker, synthwave, dracula)" RESET "\n",
        "  --color1 [hex]     Sets the starting color of the gradient (e.g., #FF0000)\n",
        "  --color2 [hex]     Sets the ending color of the gradient (e.g., #FFA500)\n",
        "  --quantized        កង់ទីសកម្មពណ៌ " MSG_CMD_DIM "(ដំណើរការខ្ពស់ជាងមុន)" RESET "\n",
        "  --spin             បង្កើតកូដ ANSI សុទ្ធ " MSG_CMD_DIM "(សម្រាប់ស្គ្រីប)" RESET "\n",
        "  --lang [id]        បដិសេធភាសាចំណុចប្រទាក់ " MSG_CMD_DIM "(ឧ. pt, en)" RESET "\n",
        "  --license          បង្ហាញលក្ខខណ្ឌអាជ្ញាប័ណ្ណកម្មវិធី\n",
        "  -v, --version      បង្ហាញកំណែគោលពីរ និងស្ថានភាពបង្កើត\n",
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
        MSG_SUCESSO "[ OK ]" RESET "\n",
        MSG_ERRO "[ FAIL ]" RESET "\n",
        MSG_AVISO "[ ⚠️ ព្រមាន ]" RESET " បរាជ័យក្នុងការផ្ទុកសោផ្ទាល់ខ្លួន ដោយប្រើសោដែលភ្ជាប់មកជាមួយ។\n"
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

    // Cálculo automático para não precisar atualizar o loop ao adicionar mais idiomas
    int num_langs = sizeof(lang_prefixes) / sizeof(lang_prefixes[0]);
    
    for (int i = 0; i < num_langs; i++) {
        if (strncmp(prefix, lang_prefixes[i], 2) == 0) {
            idioma_atual = i;
            return;
        }
    }
    
    idioma_atual = 1; // Fallback se não encontrar o idioma digitado/detectado
}

void msgs_init(void) {
    const char *lang = NULL;
#ifdef _WIN32
    lang = msgs_detect_windows_locale();
#else
    lang = getenv("LANG"); // Funciona para "pt_BR.UTF-8", ele vai pegar só o "pt" na msgs_set_language
#endif
    
    msgs_set_language(lang);
}
