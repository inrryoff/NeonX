#include "msgs.h"
#include <stdlib.h>
#include <string.h>

int idioma_atual = 0;

const char *mensagens[2][MSG_TOTAL] = {
    // ---------------- PORTUGUÊS (0) ----------------
    {
        "Erro ao abrir arquivo",
        
        // Versão
        "NeonX (versao modificada)\n",
        "Criador Original: %s\n",
        "Compilado por: %s\n",
        "Status: OFICIAL_BY_INRRYOFF\n",
        "Status: MODIFICADO (nao oficial)\n",
        "DESCONHECIDO",

        // Licença
        "LICENÇA DE USO - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2024 @inrryoff - Licenciado sob condições especiais NeonX LICENSE\n\n"
        "Pelo presente, fica concedida permissão a qualquer pessoa que obtenha uma cópia\n"
        "deste software para usá-lo gratuitamente, sujeito às seguintes condições:\n\n"
        "1. ATRIBUIÇÃO (CRÉDITOS):\n"
        "   O nome do autor original (@inrryoff) e os avisos de copyright devem ser\n"
        "   mantidos em todos os arquivos de código-fonte, cabeçalhos e na saída de\n"
        "   versão do binário compilado (ex: neonx --version).\n\n"
        "2. PROIBIÇÃO DE COMERCIALIZAÇÃO:\n"
        "   É TERMINANTEMENTE PROIBIDA a venda, aluguel ou qualquer forma de\n"
        "   comercialização deste software, seja do código-fonte ou do binário\n"
        "   compilado, de forma isolada ou integrada a pacotes pagos.\n\n"
        "3. DERIVAÇÕES E MODIFICAÇÕES:\n"
        "   Alterações no código são permitidas para melhorias ou uso pessoal, desde que:\n"
        "   a) O trabalho derivado NÃO seja vendido.\n"
        "   b) A versão modificada seja mantida em repositório público (Open Source).\n"
        "   c) Os créditos ao autor original sejam mantidos de forma clara.\n\n"
        "4. DISTRIBUIÇÃO EM MÓDULOS (MAGISK/KERNELSU):\n"
        "   O uso deste binário em módulos de otimização é permitido e encorajado,\n"
        "   desde que o módulo seja distribuído gratuitamente.\n\n"
        "O SOFTWARE É FORNECIDO 'COMO ESTÁ', SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU\n"
        "IMPLÍCITA. EM NENHUM EVENTO O AUTOR SERÁ RESPONSÁVEL POR QUALQUER RECLAMAÇÃO,\n"
        "DANOS OU OUTRA RESPONSABILIDADE RESULTANTE DO USO DESTE SOFTWARE.\n",

        // Ajuda
        "NeonX v%s | Core por: %s | Build por: %s\n\n",
        "Uso: cat arquivo | neonx [opcoes]\n\n",
        "-m [0-11]      Modos de animação\n",
        "-s [valor]     Velocidade (0.2 padrao)\n",
        "-f [valor]     Frequência (0.3 padrao)\n",
        "-d [valor]     Duração (0: infinito)\n",
        "-A [angulo]    Ângulo do gradiente (0-360 graus)\n",
        "-p [valor]     Seeds fixas\n",
        "-S             Modo estático\n",
        "-c [largura]   Largura fixa do gradiente\n",
        "-o [0-1]       Opacidade das bordas\n",
        "-F [valor]     FPS (ex: 60, 90)\n",
        "-L             Modo linha por linha (stream)\n",
        "--preset [nome] Carregar preset (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    Modo quantizado perde qualidade visual\n",
        "--spin         Gera códigos de cores ANSI puros para scripts externos\n",
        "--lang [idioma] Selecione Português ou Inglês\n",
        "--license      Licença de software\n",
        "-v,--version   Versão do binário\n",
        "-h,--help      Exibe esta ajuda\n",

        // Erros
        "Erro: A opcao '%s' exige um valor numerico apos ela.\n",
        "Erro: A opcao '%s' exige um valor numerico, recebido: '%s'\n",
        "Erro: O modo de animacao (-m) deve ser entre 0 e 11.\n",
        "Erro: Opção inválida ou argumento solto '%s'\n"
    },

    // ---------------- ENGLISH (1) ----------------
    {
        "Error opening file",
        
        // Version
        "NeonX (modified version)\n",
        "Original Creator: %s\n",
        "Compiled by: %s\n",
        "Status: OFFICIAL_BY_INRRYOFF\n",
        "Status: MODIFIED (unofficial)\n",
        "UNKNOWN",

        // License
        "USAGE LICENSE - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2024 @inrryoff - Licensed under special conditions NeonX LICENSE\n\n"
        "Permission is hereby granted to any person obtaining a copy\n"
        "of this software to use it free of charge, subject to the following conditions:\n\n"
        "1. ATTRIBUTION (CREDITS):\n"
        "   The original author's name (@inrryoff) and copyright notices must be\n"
        "   kept in all source code files, headers, and in the version output\n"
        "   of the compiled binary (e.g., neonx --version).\n\n"
        "2. NO COMMERCIALIZATION:\n"
        "   It is STRICTLY PROHIBITED to sell, rent, or commercialize this\n"
        "   software in any form, whether source code or compiled binary,\n"
        "   standalone or integrated into paid packages.\n\n"
        "3. DERIVATIONS AND MODIFICATIONS:\n"
        "   Code alterations are permitted for improvements or personal use, provided that:\n"
        "   a) The derivative work is NOT sold.\n"
        "   b) The modified version is kept in a public repository (Open Source).\n"
        "   c) Credits to the original author are clearly maintained.\n\n"
        "4. DISTRIBUTION IN MODULES (MAGISK/KERNELSU):\n"
        "   Using this binary in optimization modules is permitted and encouraged,\n"
        "   provided the module is distributed for free.\n\n"
        "THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR\n"
        "OTHER LIABILITY ARISING FROM THE USE OF THIS SOFTWARE.\n",

        // Help
        "NeonX v%s | Core by: %s | Build by: %s\n\n",
        "Usage: cat file | neonx [options]\n\n",
        "-m [0-11]      Animation modes\n",
        "-s [value]     Speed (0.2 default)\n",
        "-f [value]     Frequency (0.3 default)\n",
        "-d [value]     Duration (0: infinite)\n",
        "-A [angle]     Gradient angle (0-360 degrees)\n",
        "-p [value]     Fixed seeds\n",
        "-S             Static mode\n",
        "-c [width]     Fixed gradient width\n",
        "-o [0-1]       Edge opacity\n",
        "-F [value]     FPS (e.g., 60, 90)\n",
        "-L             Line-by-line mode (stream)\n",
        "--preset [name] Load preset (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    Using quantized mode may reduce visual quality.\n",
        "--spin         Output raw ANSI color codes for external scripts\n",
        "--lang [Language] Select Portuguese or English\n",
        "--license      Software license\n",
        "-v,--version   Binary version\n",
        "-h,--help      Display this help\n",

       // Errors
        "Error: Option '%s' requires a numeric value after it.\n",
        "Error: Option '%s' requires a numeric value, received: '%s'\n",
        "Error: Animation mode (-m) must be between 0 and 11.\n",
        "Error: Invalid option or loose argument '%s'\n"
    }
};

void msgs_init(void) {
    char *lang = getenv("LANG");
    if (lang != NULL) {
        if (strncmp(lang, "pt", 2) == 0) {
            idioma_atual = 0;
        } else {
            idioma_atual = 1;
        }
    } else {
        idioma_atual = 0;
    }
}
