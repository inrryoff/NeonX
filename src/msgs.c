#include "msgs.h"
#include <stdlib.h>
#include <string.h>

int idioma_atual = 0;

// Agora a matriz suporta 4 idiomas (0: PT, 1: EN, 2: ES, 3: ZH)
const char *mensagens[4][MSG_TOTAL] = {
    // ---------------- PORTUGUÊS (0) ----------------
    {
        "Erro ao abrir arquivo",
        
        // Versão
        "Criador Original: %s\n",
        "Compilado por: %s\n",
        "Status: OFICIAL_BY_INRRYOFF\n",
        "Status: MODIFICADO\n",

        // Licença
        "LICENÇA DE USO - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - Licenciado sob condições especiais NeonX LICENSE\n\n"
        "Pelo presente, fica concedida permissão a qualquer pessoa que obtenha uma cópia\n"
        "deste software para usá-lo gratuitamente, sujeito às seguintes condições:\n\n"
        "1. ATRIBUIÇÃO (CRÉDITOS):\n"
        "   O nome do autor original (@inrryoff) and os avisos de copyright devem ser\n"
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
        "--lang [idioma] Selecione o idioma (pt, en, es, zh)\n",
        "--license      Licença de software\n",
        "-v,--version   Versão do binário\n",
        "-h,--help      Exibe esta ajuda\n",

        // Erros
        "[NeonX Erro]: A opcao '%s' exige um valor numerico apos ela.\n",
        "[NeonX Erro]: A opcao '%s' exige um valor numerico, recebido: '%s'\n",
        "[NeonX Erro]: O modo de animacao (-m) deve ser entre 0 e 11.\n",
        "[NeonX Erro]: Opção inválida ou argumento solto '%s'\n",
        "\n\033[1;31m[NeonX Erro]: Ei! Você por acaso não leu as instruções de uso ou tá querendo quebrar o binário de propósito? 😤\033[0m\n"
        "\033[1;33m[NeonX Info]: Arquivos de log ou com mais de 1024 linhas DEVEM usar a flag -L.\033[0m\n"
        "\033[1;36m[NeonX Salvação]: Forçando modo Stream (-L) na marra para não crashar... Hum.\033[0m\n\n"
    },

    // ---------------- ENGLISH (1) ----------------
    {
        "Error opening file",
        
        // Versão
        "Original Creator: %s\n",
        "Compiled by: %s\n",
        "Status: OFFICIAL_BY_INRRYOFF\n",
        "Status: MODIFIED\n",

        // Licença
        "USAGE LICENSE - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - Licensed under special conditions NeonX LICENSE\n\n"
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

        // Ajuda
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
        "--lang [lang]  Select language (pt, en, es, zh)\n",
        "--license      Software license\n",
        "-v,--version   Binary version\n",
        "-h,--help      Display this help\n",

       // Erros
        "[NeonX Error]: Option '%s' requires a numeric value after it.\n",
        "[NeonX Error]: Option '%s' requires a numeric value, received: '%s'\n",
        "[NeonX Error]: Animation mode (-m) must be between 0 and 11.\n",
        "[NeonX Error]: Invalid option or loose argument '%s'\n",
        "\n\033[1;31m[NeonX Error]: Hey! Did you not read the usage instructions or are you trying to break the binary on purpose? 😤\033[0m\n"
        "\033[1;33m[NeonX Info]: Log files or files with more than 1024 lines MUST use the -L flag.\033[0m\n"
        "\033[1;36m[NeonX Salvation]: Forcing Stream mode (-L) right now to prevent crash... Huh.\033[0m\n\n"
    },

    // ---------------- ESPANHOL (2) ----------------
    {
        "Error al abrir el archivo",
        
        // Versão
        "Creador Original: %s\n",
        "Compilado por: %s\n",
        "Estado: OFICIAL_BY_INRRYOFF\n",
        "Estado: MODIFICADO\n",

        // Licença
        "LICENCIA DE USO - NEONX (VERSIÓN C)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - Licenciado bajo condiciones especiales NeonX LICENSE\n\n"
        "Por la presente se concede permiso a cualquier persona que obtenga una copia\n"
        "de este software para usarlo de forma gratuita, sujeto a las siguientes condiciones:\n\n"
        "1. ATRIBUCIÓN (CRÉDITOS):\n"
        "   El nombre del autor original (@inrryoff) y los avisos de derechos de autor deben\n"
        "   mantenerse en todos los archivos de código fuente, cabeceras y en la salida de\n"
        "   versión del binario compilado (ej: neonx --version).\n\n"
        "2. PROHIBICIÓN DE COMERCIALIZACIÓN:\n"
        "   Queda TERMINANTEMENTE PROHIBIDA la venta, alquiler o cualquier forma de\n"
        "   comercialización de este software, ya sea del código fuente o del binario\n"
        "   compilado, de forma aislada o integrada en paquetes de pago.\n\n"
        "3. DERIVACIONES Y MODIFICACIONES:\n"
        "   Se permiten alteraciones en el código para mejoras o uso personal, siempre que:\n"
        "   a) El trabajo derivado NO se venda.\n"
        "   b) La versión modificada se mantenga en un repositorio público (Open Source).\n"
        "   c) Los créditos al autor original se mantengan de forma clara.\n\n"
        "4. DISTRIBUCIÓN EN MÓDULOS (MAGISK/KERNELSU):\n"
        "   El uso de este binario en módulos de optimización está permitido y se fomenta,\n"
        "   siempre que el módulo se distribuya de forma gratuita.\n\n"
        "EL SOFTWARE SE PROPORCIONA 'TAL CUAL', SIN GARANTÍA DE NINGÚN TIPO, EXPRESA O\n"
        "IMPLÍCITA. EN NINGÚN CASO EL AUTOR SERÁ RESPONSABLE DE NINGUNA RECLAMACIÓN,\n"
        "DAÑOS U OTRA RESPONSABILIDAD QUE JURIDIQUE DEL USO DE ESTE SOFTWARE.\n",

        // Ajuda
        "NeonX v%s | Core por: %s | Build por: %s\n\n",
        "Uso: cat archivo | neonx [opciones]\n\n",
        "-m [0-11]      Modos de animación\n",
        "-s [valor]     Velocidad (0.2 por defecto)\n",
        "-f [valor]     Frecuencia (0.3 por defecto)\n",
        "-d [valor]     Duración (0: infinito)\n",
        "-A [angulo]    Ángulo del gradiente (0-360 grados)\n",
        "-p [valor]     Semillas fijas (Seeds)\n",
        "-S             Modo estático\n",
        "-c [ancho]     Ancho fijo del gradiente\n",
        "-o [0-1]       Opacidad de los bordes\n",
        "-F [valor]     FPS (ej: 60, 90)\n",
        "-L             Modo línea por línea (stream)\n",
        "--preset [nom] Cargar preset (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    El modo cuantizado reduce la calidad visual\n",
        "--spin         Genera códigos de color ANSI puros para scripts externos\n",
        "--lang [idiom] Seleccione el idioma (pt, en, es, zh)\n",
        "--license      Licencia de software\n",
        "-v,--version   Versión del binario\n",
        "-h,--help      Muestra esta ayuda\n",

       // Erros
        "[NeonX Error]: La opción '%s' requiere un valor numérico después de ella.\n",
        "[NeonX Error]: La opción '%s' requiere un valor numérico, recibido: '%s'\n",
        "[NeonX Error]: El modo de animación (-m) debe estar entre 0 y 11.\n",
        "[NeonX Error]: Opción inválida o argumento suelto '%s'\n",
        "\n\033[1;31m[NeonX Error]: ¡Oye! ¿Acaso no leíste las instrucciones de uso o quieres romper el binario a propósito? 😤\033[0m\n"
        "\033[1;33m[NeonX Info]: Archivos de log o con más de 1024 líneas DEBEN usar la bandera -L.\033[0m\n"
        "\033[1;36m[NeonX Salvación]: Forzando modo Stream (-L) ahora mismo para evitar crash... Ajá.\033[0m\n\n"
    },

    // ---------------- CHINÊS (3) ----------------
    {
        "无法打开文件",
        
        // Versão
        "原作者: %s\n",
        "编译者: %s\n",
        "状态: 官方正版_BY_INRRYOFF\n",
        "状态: 已修改\n",

        // Licença
        "使用许可 - NEONX (C语言版本)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - 在 NeonX LICENSE 特殊条件下授权\n\n"
        "特此授权任何获得本软件副本的人员免费使用，但须符合以下条件:\n\n"
        "1. 署名 (积分/限额):\n"
        "   原作者姓名 (@inrryoff) 和版权声明必须保留在所有源码文件、\n"
        "   头文件以及编译后的二进制版本输出中 (例如: neonx --version)。\n\n"
        "2. 禁止商业化:\n"
        "   严禁以任何形式销售、出租或商业化本软件 (无论是源码还是编译后的二进制文件)，\n"
        "   无论是独立销售还是集成到收费包中。\n\n"
        "3. 衍生与修改:\n"
        "   允许为了改进或个人使用而修改代码，前提是:\n"
        "   a) 衍生作品不得销售。\n"
        "   b) 修改后的版本必须保存在公开仓库中 (开源)。\n"
        "   c) 必须清晰保留对原作者的致谢。\n\n"
        "4. 模块分发 (MAGISK/KERNELSU):\n"
        "   允许并鼓励在优化模块中使用此二进制文件，前提是该模块免费分发。\n\n"
        "本软件按“原样”提供，不提供任何形式 of 明示或暗示保证。在任何情况下，\n"
        "作者均不对因使用本软件而引起的任何索赔、损害或其他责任负责。\n",

        // Ajuda
        "NeonX v%s | 核心设计: %s | 编译版本: %s\n\n",
        "用法: cat 文件 | neonx [选项]\n\n",
        "-m [0-11]      动画模式\n",
        "-s [数值]      速度 (默认 0.2)\n",
        "-f [数值]      频率 (默认 0.3)\n",
        "-d [数值]      持续时间 (0: 无限)\n",
        "-A [角度]      渐变角度 (0-360 度)\n",
        "-p [数值]      固定随机种子\n",
        "-S             静态模式\n",
        "-c [宽度]      固定渐变宽度\n",
        "-o [0-1]       边缘透明度 (边框不透明度)\n",
        "-F [数值]      帧率 FPS (例如: 60, 90)\n",
        "-L             逐行模式 (流模式 - stream)\n",
        "--preset [名]  加载预设 (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    量化模式会降低视觉质量\n",
        "--spin         为外部脚本输出纯 ANSI 颜色代码\n",
        "--lang [语言]  选择语言 (pt, en, es, zh)\n",
        "--license      软件许可证\n",
        "-v,--version   二进制版本信息\n",
        "-h,--help      显示此帮助信息\n",

       // Erros
        "[NeonX 错误]: 选项 '%s' 后面需要跟一个数值。\n",
        "[NeonX 错误]: 选项 '%s' 需要数值，却收到: '%s'\n",
        "[NeonX 错误]: 动画模式 (-m) 必须在 0 到 11 之间。\n",
        "[NeonX 错误]: 无效选项或孤立参数 '%s'\n",
        "\n\033[1;31m[NeonX 错误]: 喂！你是不是没看使用说明，还是故意想把二进制搞崩溃？ 😤\033[0m\n"
        "\033[1;33m[NeonX 提示]: 日志文件或超过 1024 行的文件必须使用 -L 参数。\033[0m\n"
        "\033[1;36m[NeonX 自动拯救]: 正在强制启用流模式 (-L) 以防崩溃... 哼。\033[0m\n\n"
    }
};

void msgs_init(void) {
    char *lang = getenv("LANG");
    if (lang != NULL) {
        if (strncmp(lang, "pt", 2) == 0) {
            idioma_atual = 0; // Português
        } else if (strncmp(lang, "es", 2) == 0) {
            idioma_atual = 2; // Español
        } else if (strncmp(lang, "zh", 2) == 0) {
            idioma_atual = 3; // Chinese (Mandarin)
        } else {
            idioma_atual = 1; // English (Fallback para o resto)
        }
    } else {
        idioma_atual = 0;
    }
}
