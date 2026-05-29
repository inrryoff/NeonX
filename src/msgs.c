#include "msgs.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

static int idioma_atual = 0;
typedef struct {
    const char *code;
    int index;
} LanguageMap;

static const LanguageMap lang_map[] = {
    {"pt", 0}, {"en", 1}, {"es", 2}, {"zh", 3},
    {"ja", 4}, {"ar", 5}, {"ru", 6}, {"bg", 7},
    {"el", 8}, {"ko", 9}, {"hi",10}, {"th",11},
    {"km",12}
};
static const int lang_map_size = sizeof(lang_map) / sizeof(lang_map[0]);

static int lookup_language(const char *code) {
    if (!code) return -1;
    for (int i = 0; i < lang_map_size; i++) {
        if (strncmp(code, lang_map[i].code, 2) == 0)
            return lang_map[i].index;
    }
    return -1;
}

static const char *mensagens[13][MSG_TOTAL] = {
    // ---------------- PORTUGUÊS (0) ----------------
    {
        "Erro ao abrir arquivo",

        // Versão
        "Criador Original: ",
        "Compilado por: ",
        "Status: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "Status: \033[1;31mMODIFICADO\033[0m\n",
        "Status: \033[1;33mVERIFY_ERROR\033[0m\n",

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
        "NeonX",
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

        //Erros
        "\033[1;31m[NeonX Erro 400]: A opção '%s' requer um valor numérico após ela.\033[0m\n",
        "\033[1;31m[NeonX Erro 400]: A opção '%s' requer um valor numérico, recebido: '%s'\033[0m\n",
        "\033[1;31m[NeonX Erro 400]: O modo de animação (-m) deve ser um número inteiro.\033[0m\n",
        "\033[1;33m[NeonX Info 416]: O modo de animação (-m) deve estar entre 0 e 11.\033[0m\n",
        "\033[1;31m[NeonX Erro 400]: Opção inválida ou argumento solto '%s'\033[0m\n",
        "\n\033[1;31m[NeonX Erro 404]: Nenhum dado foi passado para o binário! \033[0m\n",
        "\n\033[1;31m[NeonX Erro 413]: Arquivo muito grande. Use -L para o modo de streaming.\033[0m\n",
        "\033[1;33m[NeonX Info 403]: Não foi possível verificar a integridade, sistema restrito ou binário adulterado.\033[0m\n",
        "\033[1;31m[NeonX Erro 403]: Falha ao abrir o próprio executável para verificação de integridade.\033[0m\n",
        "\033[1;31m[NeonX Erro 403]: Erro de leitura do arquivo durante a verificação de integridade.\033[0m\n",
        "\033[1;31m[NeonX Erro 403]: Memória insuficiente para verificar a integridade.\033[0m\n",
        "\033[1;31m[NeonX Erro 403]: Assinatura inválida ou formato hexadecimal incorreto.\033[0m\n",
        "\033[1;31m[NeonX Erro 403]: O arquivo é muito pequeno para conter a assinatura de integridade.\033[0m\n",
        "\033[1;31m[NeonX Erro 400]: A opção '%s' requer um número inteiro.\033[0m\n",
        "\033[1;31m[NeonX Erro 400]: A opção '%s' requer um valor positivo.\033[0m\n",
        "\033[1;31m[NeonX Erro 400]: A duração não pode ser negativa.\033[0m\n",
        "\033[1;31m[NeonX Erro 206]: Não foi possível alocar memória (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX Info 403]: Buffer truncado preventivamente para 32MB. A renderização pode ser corrompida.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX Aviso]: Falha ao carregar chave customizada, usando chave embutida.\033[0m\n"
    },

    // ---------------- ENGLISH (1) ----------------
    {
        "\033[1;31mError opening file\033[0m",

        // Versão
        "Original Creator: ",
        "Compiled by: ",
        "Status: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "Status: \033[1;31mMODIFIED\033[0m\n",
        "Status: \033[1;33mVERIFY_ERROR\033[0m\n",

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
        "NeonX",
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
        "\033[1;31m[NeonX Error 400]: The option '%s' requires a numeric value after it.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: The option '%s' requires a numeric value, received: '%s'\033[0m\n",
        "\033[1;31m[NeonX Error 400]: The animation mode (-m) must be an integer.\033[0m\n",
        "\033[1;33m[NeonX Info 416]: The animation mode (-m) must be between 0 and 11.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: Invalid option or loose argument '%s'\033[0m\n",
        "\n\033[1;31m[NeonX Error 404]: No data was passed to the binary! \033[0m\n",
        "\n\033[1;31m[NeonX Error 413]: File too large. Use -L for streaming mode.\033[0m\n",
        "\033[1;33m[NeonX Info 403]: Could not check integrity, restricted system or tampered binary.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Failed to open own executable for integrity check.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: File read error during integrity check.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Insufficient memory to verify integrity.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Invalid signature or incorrect hexadecimal format.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: The file is too small to contain the integrity signature.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: The option '%s' requires an integer.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: The option '%s' requires a positive value.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: Duration cannot be negative.\033[0m\n",
        "\033[1;31m[NeonX Error 206]: Could not allocate memory (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX Info 403]: Buffer truncated to 32MB. Rendering may be corrupted.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX Warning]: Failed to load custom key, using built-in fallback.\033[0m\n"
    },

    // ---------------- ESPANHOL (2) ----------------
    {
        "\033[1;31mError al abrir el archivo\033[0m",

        // Versão
        "Creador Original: ",
        "Compilado por: ",
        "Estado: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "Estado: \033[1;31mMODIFICADO\033[0m\n",
        "Estado: \033[1;33mVERIFY_ERROR\033[0m\n",

        // Licença
        "LICENCIA DE USO - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - Licenciado bajo condiciones especiales NeonX LICENSE\n\n"
        "Por la presente se concede permiso a cualquier persona que obtenga una copia\n"
        "de este software para utilizarlo de forma gratuita, sujeto a las siguientes condiciones:\n\n"
        "1. ATRIBUCIÓN (CRÉDITOS):\n"
        "   El nombre del autor original (@inrryoff) y los avisos de derechos de autor deben\n"
        "   mantenerse en todos los archivos de código fuente, encabezados y en la salida de\n"
        "   versión del binario compilado (ej: neonx --version).\n\n"
        "2. PROHIBICIÓN DE COMERCIALIZACIÓN:\n"
        "   Queda TERMINANTEMENTE PROHIBIDA la venta, alquiler o cualquier forma de\n"
        "   comercialización de este software, ya sea del código fuente o del binario\n"
        "   compilado, de forma aislada o integrada en paquetes de pago.\n\n"
        "3. DERIVACIONES Y MODIFICACIONES:\n"
        "   Se permiten modificaciones del código para mejoras o uso personal, siempre que:\n"
        "   a) El trabajo derivado NO se venda.\n"
        "   b) La versión modificada se mantenga en un repositorio público (Open Source).\n"
        "   c) Los créditos al autor original se mantengan de forma clara.\n\n"
        "4. DISTRIBUCIÓN EN MÓDULOS (MAGISK/KERNELSU):\n"
        "   El uso de este binario en módulos de optimización está permitido y se fomenta,\n"
        "   siempre que el módulo se distribuya de forma gratuita.\n\n"
        "EL SOFTWARE SE PROPORCIONA 'TAL CUAL', SIN GARANTÍA DE NINGÚN TIPO, EXPRESA O\n"
        "IMPLÍCITA. EN NINGÚN CASO EL AUTOR SERÁ RESPONSABLE DE NINGUNA RECLAMACIÓN,\n"
        "DAÑOS U OTRA RESPONSABILIDAD QUE SURJA DEL USO DE ESTE SOFTWARE.\n",

        // Ajuda
        "NeonX",
        "Uso: cat archivo | neonx [opciones]\n\n",
        "-m [0-11]      Modos de animación\n",
        "-s [valor]     Velocidad (0.2 por defecto)\n",
        "-f [valor]     Frecuencia (0.3 por defecto)\n",
        "-d [valor]     Duración (0: infinito)\n",
        "-A [angulo]    Ángulo del gradiente (0-360 grados)\n",
        "-p [valor]     Seeds fijas\n",
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
        "\033[1;31m[NeonX Error 400]: La opción '%s' requiere un valor numérico después de ella.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: La opción '%s' requiere un valor numérico, recibido: '%s'\033[0m\n",
        "\033[1;31m[NeonX Error 400]: El modo de animación (-m) debe ser un número entero.\033[0m\n",
        "\033[1;33m[NeonX Info 416]: El modo de animación (-m) debe estar entre 0 y 11.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: Opción inválida o argumento suelto '%s'\033[0m\n",
        "\n\033[1;31m[NeonX Error 404]: ¡No se pasaron datos al binario! \033[0m\n",
        "\n\033[1;31m[NeonX Error 413]: Archivo demasiado grande. Use -L para modo streaming.\033[0m\n",
        "\033[1;33m[NeonX Info 403]: No se pudo verificar la integridad, sistema restringido o binario alterado.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Error al abrir el propio ejecutable para la verificación de integridad.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Error en la lectura del archivo durante la verificación de integridad.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Memoria insuficiente para verificar la integridad.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: Firma inválida o formato hexadecimal incorrecto.\033[0m\n",
        "\033[1;31m[NeonX Error 403]: El archivo es demasiado pequeño para contener la firma de integridad.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: La opción '%s' requiere un número entero.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: La opción '%s' requiere un valor positivo.\033[0m\n",
        "\033[1;31m[NeonX Error 400]: La duración no puede ser negativa.\033[0m\n",
        "\033[1;31m[NeonX Error 206]: No se pudo asignar memoria (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX Info 403]: Buffer truncado a 32MB. La representación puede corromperse.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX Aviso]: Error ao cargar la clave personalizada, usando fallback.\033[0m\n"
    },

    // ---------------- CHINÊS (3) ----------------
    {
        "打开文件失败",

        // 核心版本信息
        "原作者: ",
        "编译者: ",
        "状态: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "状态: \033[1;31mMODIFICADO\033[0m\n", // 保留底层标识或翻译为 已修改
        "状态: \033[1;33mVERIFY_ERROR\033[0m\n",

        // 许可证
        "使用许可证 - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - 依据 NeonX LICENSE 特别条件授权\n\n"
        "特此授权任何获得本软件副本的人员免费使用，但须符合以下条件:\n\n"
        "1. 署名 (贡献人员):\n"
        "   原作者姓名 (@inrryoff) 和版权声明必须保留在所有源代码文件、\n"
        "   头文件以及编译后的二进制版本输出中 (例如: neonx --version)。\n\n"
        "2. 禁止商业化:\n"
        "   严禁出售、出租或以任何形式商业化本软件 (无论是源代码还是编译后的\n"
        "   二进制文件)，无论是单独销售还是集成到付费包中。\n\n"
        "3. 衍生作品与修改:\n"
        "   允许修改代码用于改进或个人使用，前提是:\n"
        "   a) 衍生作品不得销售。\n"
        "   b) 修改后的版本必须保存在公共仓库中 (开源)。\n"
        "   c) 必须清晰保留对原作者的署名。\n\n"
        "4. 模块分发 (MAGISK/KERNELSU):\n"
        "   允许并鼓励在优化模块中使用此二进制文件，前提是该模块必须免费分发。\n\n"
        "本软件按“原样”提供，不提供任何明示或暗示的保证。在任何情况下，\n"
        "作者均不对因使用本软件而产生的任何索赔、损害或其他责任负责。\n",

        // 帮助信息
        "NeonX",
        "用法: cat 文件 | neonx [选项]\n\n",
        "-m [0-11]      动画模式\n",
        "-s [数值]     速度 (默认 0.2)\n",
        "-f [数值]     频率 (默认 0.3)\n",
        "-d [数值]     时长 (0: 无限)\n",
        "-A [角度]    渐变角度 (0-360 度)\n",
        "-p [数值]     固定种子\n",
        "-S             静态模式\n",
        "-c [宽度]     固定渐变宽度\n",
        "-o [0-1]       边缘透明度\n",
        "-F [数值]     FPS (例如: 60, 90)\n",
        "-L             逐行模式 (流式传输)\n",
        "--preset [名称] 加载预设 (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    量化模式会降低视觉质量\n",
        "--spin         为外部脚本生成纯 ANSI 颜色代码\n",
        "--lang [语言] 选择葡萄牙语或英语\n",
        "--license      软件许可证\n",
        "-v,--version   二进制文件版本\n",
        "-h,--help      显示此帮助信息\n",

        // 错误与通知
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 后面需要一个数值。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 需要一个数值，收到: '%s'\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 动画模式 (-m) 必须是整数。\033[0m\n",
        "\033[1;33m[NeonX 信息 416]: 动画模式 (-m) 必须介于 0 和 11 之间。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 无效选项或孤立参数 '%s'\033[0m\n",
        "\n\033[1;31m[NeonX 错误 404]: 未向二进制文件传递任何数据！\033[0m\n",
        "\n\033[1;31m[NeonX 错误 413]: 文件过大。请使用 -L 进行流式传输模式。\033[0m\n",
        "\033[1;33m[NeonX 信息 403]: 无法验证完整性，系统受限或二进制文件已被篡改。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 无法打开可执行文件本身以进行完整性检查。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 完整性检查期间读取文件错误。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 内存不足，无法验证完整性。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 签名无效或十六进制格式错误。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 文件太小，无法包含完整性签名。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 需要一个整数。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 需要一个正值。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 时长不能为负数。\033[0m\n",
        "\033[1;31m[NeonX 错误 206]: 无法分配内存 (wcsdup)。\033[0m\n",
        "\033[1;33m[NeonX 信息 403]: 缓冲区已自动截断至 32MB。渲染可能会损坏。\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX 警告]: 加载自定义密钥失败，正在使用内置密钥。\033[0m\n"
    },


    // ---------------- JAPONÊS (4) ----------------
    {
        "ファイルを開けませんでした",

        // Versão
        "オリジナル作者: ",
        "コンパイル: ",
        "ステータス: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "ステータス: \033[1;31mMODIFIED\033[0m\n",
        "ステータス: \033[1;33mVERIFY_ERROR\033[0m\n",

        // Licença
        "使用ライセンス - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - NeonXライセンス特別条件の下でライセンスされます\n\n"
        "本ソフトウェアのコピーを入手するすべての人は、以下の条件に従い、無料で使用する許可が与えられます。\n\n"
        "1. 帰属（クレジット）:\n"
        "   原作者の名前（@inrryoff）および著作権表示は、すべてのソースコードファイル、ヘッダー、\n"
        "   コンパイル済みバイナリのバージョン出力（例: neonx --version）に保持されなければなりません。\n\n"
        "2. 営利目的の禁止:\n"
        "   ソースコードまたはコンパイル済みバイナリを問わず、本ソフトウェアを販売、賃貸、または\n"
        "   商用化することは固く禁じられています。\n\n"
        "3. 派生および変更:\n"
        "   改善や個人使用のためのコード変更は許可されています。ただし、以下の条件を満たす必要があります:\n"
        "   a) 派生作品を販売しないこと。\n"
        "   b) 変更バージョンを公開リポジトリ（オープンソース）に維持すること。\n"
        "   c) 原作者へのクレジットを明確に維持すること。\n\n"
        "4. モジュールでの配布（MAGISK/KERNELSU）:\n"
        "   最適化モジュールでの本バイナリの使用は、モジュールが無料で配布される限り許可され推奨されます。\n\n"
        "本ソフトウェアは「現状のまま」提供され、明示または黙示を問わずいかなる保証もありません。\n"
        "本ソフトウェアの使用に起因するいかなる請求、損害、その他の責任についても、著者は一切の責任を負いません。\n",

        // Ajuda
        "NeonX",
        "使用法: cat ファイル | neonx [オプション]\n\n",
        "-m [0-11]      アニメーションモード\n",
        "-s [値]        速度（デフォルト 0.2）\n",
        "-f [値]        周波数（デフォルト 0.3）\n",
        "-d [値]        持続時間（0: 無限）\n",
        "-A [角度]      グラデーション角度（0-360度）\n",
        "-p [値]        固定シード\n",
        "-S             静的モード\n",
        "-c [幅]        固定グラデーション幅\n",
        "-o [0-1]       エッジ不透明度\n",
        "-F [値]        FPS（例: 60, 90）\n",
        "-L             行ごとモード（ストリーム）\n",
        "--preset [名前] プリセットをロード（cyberpunk, retro, matrix, sunset）\n",
        "--quantized    量子化モード（画質が低下することがあります）\n",
        "--spin         外部スクリプト用の生のANSIカラーコードを出力\n",
        "--lang [言語]  言語を選択（pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km）\n",
        "--license      ソフトウェアライセンス\n",
        "-v,--version   バイナリバージョン\n",
        "-h,--help      このヘルプを表示\n",

        // エラーメッセージ
        "\033[1;31m[NeonX エラー 400]: オプション '%s' には数値が必要です。\033[0m\n",
        "\033[1;31m[NeonX エラー 400]: オプション '%s' には数値が必要です（受信: '%s'）。\033[0m\n",
        "\033[1;31m[NeonX エラー 400]: アニメーションモード (-m) は整数でなければなりません。\033[0m\n",
        "\033[1;33m[NeonX 情報 416]: アニメーションモード (-m) は 0 から 11 の間でなければなりません。\033[0m\n",
        "\033[1;31m[NeonX エラー 400]: 無効なオプションまたは孤立した引数 '%s'\033[0m\n",
        "\n\033[1;31m[NeonX エラー 404]: バイナリにデータが渡されていません！ \033[0m\n",
        "\n\033[1;31m[NeonX エラー 413]: ファイルが大きすぎます。ストリーミングモードには -L を使用してください。\033[0m\n",
        "\033[1;33m[NeonX 情報 403]: 整合性を確認できません。システムが制限されているか、バイナリが改ざんされています。\033[0m\n",
        "\033[1;31m[NeonX エラー 403]: 整合性チェックのために自身の実行ファイルを開けませんでした。\033[0m\n",
        "\033[1;31m[NeonX エラー 403]: 整合性チェック中にファイル読み取りエラーが発生しました。\033[0m\n",
        "\033[1;31m[NeonX エラー 403]: 整合性を検証するにはメモリが不足しています。\033[0m\n",
        "\033[1;31m[NeonX エラー 403]: 署名が無効であるか、16進数形式が正しくありません。\033[0m\n",
        "\033[1;31m[NeonX エラー 403]: ファイルが小さすぎて整合性署名を格納できません。\033[0m\n",
        "\033[1;31m[NeonX エラー 400]: オプション '%s' には整数が必要です。\033[0m\n",
        "\033[1;31m[NeonX エラー 400]: オプション '%s' には正の値が必要です。\033[0m\n",
        "\033[1;31m[NeonX エラー 400]: 持続時間は負にできません。\033[0m\n",
        "\033[1;31m[NeonX エラー 206]: メモリを割り当てられませんでした（wcsdup）。\033[0m\n",
        "\033[1;33m[NeonX 情報 403]: バッファーは予防的に32MBに切り詰められました。レンダリングが破損する可能性があります。\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX 警告]: カスタムキーの読み込みに失敗しました。組み込みフォールバックを使用します。\033[0m\n"
    },

    // ---------------- ÁRABE (5) ----------------
    {
        "خطأ في فتح الملف",

        // الإصدار
        "المنشئ الأصلي: ",
        "تم تجميعه بواسطة: ",
        "الحالة: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "الحالة: \033[1;31mMODIFIED\033[0m\n",
        "الحالة: \033[1;33mVERIFY_ERROR\033[0m\n",

        // الترخيص
        "رخصة الاستخدام - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "حقوق الطبع والنشر (c) 2026 @inrryoff - مرخص بموجب شروط خاصة NeonX LICENSE\n\n"
        "بموجب هذا، يُمنح أي شخص يحصل على نسخة من هذا البرنامج الإذن باستخدامه مجانًا، بشرط الالتزام بالشروط التالية:\n\n"
        "1. الإسناد (الإشادة):\n"
        "   يجب الاحتفاظ باسم المؤلف الأصلي (@inrryoff) وإشعارات حقوق الطبع والنشر في جميع ملفات المصدر، والرؤوس،\n"
        "   وإصدار الملف الثنائي المترجم (مثل neonx --version).\n\n"
        "2. حظر التسويق:\n"
        "   يُحظر تمامًا بيع أو تأجير أو تسويق هذا البرنامج بأي شكل، سواء كان كود المصدر أو الملف الثنائي المترجم،\n"
        "   بشكل منفرد أو مدمج في حزم مدفوعة.\n\n"
        "3. الاشتقاقات والتعديلات:\n"
        "   يُسمح بتغييرات الكود لأغراض التحسين أو الاستخدام الشخصي، بشرط:\n"
        "   أ) عدم بيع العمل المشتق.\n"
        "   ب) الاحتفاظ بالنسخة المعدلة في مستودع عام (مفتوح المصدر).\n"
        "   ج) الحفاظ على إشادة المؤلف الأصلي بشكل واضح.\n\n"
        "4. التوزيع في الوحدات (MAGISK/KERNELSU):\n"
        "   يُسمح ويُشجع باستخدام هذا الملف الثنائي في وحدات التحسين، بشرط توزيع الوحدة مجانًا.\n\n"
        "يتم توفير البرنامج 'كما هو'، دون أي ضمان من أي نوع، صريحًا أو ضمنيًا.\n"
        "في أي حال، لن يكون المؤلف مسؤولاً عن أي مطالبات أو أضرار أو مسؤوليات أخرى ناتجة عن استخدام هذا البرنامج.\n",

        // المساعدة
        "NeonX",
        "الاستخدام: cat ملف | neonx [خيارات]\n\n",
        "-m [0-11]      أوضاع الرسوم المتحركة\n",
        "-s [قيمة]     السرعة (الافتراضي 0.2)\n",
        "-f [قيمة]     التردد (الافتراضي 0.3)\n",
        "-d [قيمة]     المدة (0: غير محدود)\n",
        "-A [زاوية]    زاوية التدرج (0-360 درجة)\n",
        "-p [قيمة]     البذور الثابتة\n",
        "-S             الوضع الثابت\n",
        "-c [عرض]      عرض التدرج الثابت\n",
        "-o [0-1]      شفافية الحافة\n",
        "-F [قيمة]     معدل الإطارات (مثال: 60, 90)\n",
        "-L             وضع سطر بسطر (تدفق)\n",
        "--preset [اسم] تحميل إعداد مسبق (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    الوضع الكمي قد يقلل الجودة البصرية\n",
        "--spin         إخراج رموز ألوان ANSI الخام للنصوص الخارجية\n",
        "--lang [لغة]  اختر اللغة (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      ترخيص البرنامج\n",
        "-v,--version   إصدار الملف الثنائي\n",
        "-h,--help      عرض هذه المساعدة\n",

        // رسائل الخطأ
        "\033[1;31m[NeonX خطأ 400]: الخيار '%s' يتطلب قيمة رقمية بعده.\033[0m\n",
        "\033[1;31m[NeonX خطأ 400]: الخيار '%s' يتطلب قيمة رقمية، المستلم: '%s'\033[0m\n",
        "\033[1;31m[NeonX خطأ 400]: وضع الرسوم المتحركة (-m) يجب أن يكون عددًا صحيحًا.\033[0m\n",
        "\033[1;33m[NeonX معلومات 416]: وضع الرسوم المتحركة (-m) يجب أن يكون بين 0 و 11.\033[0m\n",
        "\033[1;31m[NeonX خطأ 400]: خيار غير صالح أو وسيطة منفردة '%s'\033[0m\n",
        "\n\033[1;31m[NeonX خطأ 404]: لم يتم تمرير أي بيانات إلى الملف الثنائي! \033[0m\n",
        "\n\033[1;31m[NeonX خطأ 413]: الملف كبير جدًا. استخدم -L لوضع التدفق.\033[0m\n",
        "\033[1;33m[NeonX معلومات 403]: لا يمكن التحقق من السلامة، نظام مقيد أو ملف ثنائي معدل.\033[0m\n",
        "\033[1;31m[NeonX خطأ 403]: فشل في فتح الملف التنفيذي الخاص للتحقق من السلامة.\033[0m\n",
        "\033[1;31m[NeonX خطأ 403]: خطأ في قراءة الملف أثناء التحقق من السلامة.\033[0m\n",
        "\033[1;31m[NeonX خطأ 403]: ذاكرة غير كافية للتحقق من السلامة.\033[0m\n",
        "\033[1;31m[NeonX خطأ 403]: توقيع غير صالح أو تنسيق سداسي عشري غير صحيح.\033[0m\n",
        "\033[1;31m[NeonX خطأ 403]: الملف صغير جدًا بحيث لا يمكنه احتواء توقيع السلامة.\033[0m\n",
        "\033[1;31m[NeonX خطأ 400]: الخيار '%s' يتطلب عددًا صحيحًا.\033[0m\n",
        "\033[1;31m[NeonX خطأ 400]: الخيار '%s' يتطلب قيمة موجبة.\033[0m\n",
        "\033[1;31m[NeonX خطأ 400]: لا يمكن أن تكون المدة سالبة.\033[0m\n",
        "\033[1;31m[NeonX خطأ 206]: تعذر تخصيص الذاكرة (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX معلومات 403]: تم اقتطاع المخزن المؤقت إلى 32 ميجابايت كإجراء وقائي. قد يكون العرض تالفًا.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX تحذير]: فشل تحميل المفتاح المخصص، باستخدام الاحتياطي المدمج.\033[0m\n"
    },

    // ---------------- RUSSO (6) ----------------
    {
        "Ошибка открытия файла",

        // Версия
        "Оригинальный автор: ",
        "Скомпилировано: ",
        "Статус: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "Статус: \033[1;31mMODIFIED\033[0m\n",
        "Статус: \033[1;33mVERIFY_ERROR\033[0m\n",

        // Лицензия
        "ЛИЦЕНЗИЯ НА ИСПОЛЬЗОВАНИЕ - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - Лицензировано на особых условиях NeonX LICENSE\n\n"
        "Настоящим предоставляется разрешение любому лицу, получившему копию данного программного обеспечения, использовать его бесплатно при соблюдении следующих условий:\n\n"
        "1. АТРИБУЦИЯ (БЛАГОДАРНОСТЬ):\n"
        "   Имя оригинального автора (@inrryoff) и уведомления об авторских правах должны сохраняться во всех файлах исходного кода, заголовках и выводе версии скомпилированного бинарного файла (например, neonx --version).\n\n"
        "2. ЗАПРЕТ НА КОММЕРЦИАЛИЗАЦИЮ:\n"
        "   СТРОГО ЗАПРЕЩАЕТСЯ продажа, аренда или любая форма коммерциализации данного программного обеспечения, будь то исходный код или скомпилированный бинарный файл, отдельно или в составе платных пакетов.\n\n"
        "3. ПРОИЗВОДНЫЕ И ИЗМЕНЕНИЯ:\n"
        "   Изменения кода разрешены для улучшений или личного использования при условии:\n"
        "   a) Производная работа НЕ продаётся.\n"
        "   b) Изменённая версия сохраняется в открытом репозитории (Open Source).\n"
        "   c) Благодарность оригинальному автору сохраняется чётко.\n\n"
        "4. РАСПРОСТРАНЕНИЕ В МОДУЛЯХ (MAGISK/KERNELSU):\n"
        "   Использование данного бинарного файла в оптимизационных модулях разрешено и поощряется при условии бесплатного распространения модуля.\n\n"
        "ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ» БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, ЯВНЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ.\n"
        "НИ ПРИ КАКИХ ОБСТОЯТЕЛЬСТВАХ АВТОР НЕ НЕСЁТ ОТВЕТСТВЕННОСТИ ЗА КАКИЕ-ЛИБО ПРЕТЕНЗИИ, УБЫТКИ ИЛИ ИНУЮ ОТВЕТСТВЕННОСТЬ, ВОЗНИКШИЕ В РЕЗУЛЬТАТЕ ИСПОЛЬЗОВАНИЯ ДАННОГО ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ.\n",

        // Справка
        "NeonX",
        "Использование: cat файл | neonx [опции]\n\n",
        "-m [0-11]      Режимы анимации\n",
        "-s [значение]  Скорость (по умолчанию 0.2)\n",
        "-f [значение]  Частота (по умолчанию 0.3)\n",
        "-d [значение]  Длительность (0: бесконечно)\n",
        "-A [угол]      Угол градиента (0-360 градусов)\n",
        "-p [значение]  Фиксированные семена\n",
        "-S             Статический режим\n",
        "-c [ширина]    Фиксированная ширина градиента\n",
        "-o [0-1]       Прозрачность краёв\n",
        "-F [значение]  FPS (например, 60, 90)\n",
        "-L             Построчный режим (поток)\n",
        "--preset [имя] Загрузить пресет (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    Квантованный режим может ухудшить визуальное качество\n",
        "--spin         Вывод сырых ANSI-кодов цветов для внешних скриптов\n",
        "--lang [язык]  Выберите язык (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      Лицензия программного обеспечения\n",
        "-v,--version   Версия бинарного файла\n",
        "-h,--help      Показать эту справку\n",

        // Сообщения об ошибках
        "\033[1;31m[NeonX Ошибка 400]: Опция '%s' требует числового значения после себя.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 400]: Опция '%s' требует числового значения, получено: '%s'\033[0m\n",
        "\033[1;31m[NeonX Ошибка 400]: Режим анимации (-m) должен быть целым числом.\033[0m\n",
        "\033[1;33m[NeonX Инфо 416]: Режим анимации (-m) должен быть от 0 до 11.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 400]: Неверная опция или отдельный аргумент '%s'\033[0m\n",
        "\n\033[1;31m[NeonX Ошибка 404]: В бинарный файл не передано никаких данных! \033[0m\n",
        "\n\033[1;31m[NeonX Ошибка 413]: Файл слишком большой. Используйте -L для потокового режима.\033[0m\n",
        "\033[1;33m[NeonX Инфо 403]: Не удалось проверить целостность: ограниченная система или изменённый бинарный файл.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 403]: Не удалось открыть собственный исполняемый файл для проверки целостности.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 403]: Ошибка чтения файла во время проверки целостности.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 403]: Недостаточно памяти для проверки целостности.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 403]: Неверная подпись или неправильный шестнадцатеричный формат.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 403]: Файл слишком мал, чтобы содержать подпись целостности.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 400]: Опция '%s' требует целого числа.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 400]: Опция '%s' требует положительного значения.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 400]: Длительность не может быть отрицательной.\033[0m\n",
        "\033[1;31m[NeonX Ошибка 206]: Не удалось выделить память (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX Инфо 403]: Буфер профилактически урезан до 32 МБ. Рендеринг может быть повреждён.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX Предупреждение]: Не удалось загрузить пользовательский ключ, используется встроенный запасной.\033[0m\n"
    },

    // ---------------- BÚLGARO (7) ----------------
    {
        "Грешка при отваряне на файл",

        // Версия
        "Оригинален създател: ",
        "Компилиран от: ",
        "Статус: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "Статус: \033[1;31mMODIFIED\033[0m\n",
        "Статус: \033[1;33mVERIFY_ERROR\033[0m\n",

        // Лиценз
        "ЛИЦЕНЗ ЗА ИЗПОЛЗВАНЕ - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - Лицензирано при специални условия NeonX LICENSE\n\n"
        "С настоящето се дава разрешение на всяко лице, получило копие от този софтуер, да го използва безплатно, при спазване на следните условия:\n\n"
        "1. ПРИПИСВАНЕ (КРЕДИТИ):\n"
        "   Името на оригиналния автор (@inrryoff) и известията за авторски права трябва да се съхраняват във всички файлове с изходен код, заглавни файлове и в изхода за версия на компилирания двоичен файл (напр. neonx --version).\n\n"
        "2. ЗАБРАНА ЗА ТЪРГОВСКА УПОТРЕБА:\n"
        "   СТРОГО ЗАБРАНЯВА се продажбата, наемането или всякаква форма на търговия с този софтуер, независимо дали като изходен код или компилиран двоичен файл, самостоятелно или интегриран в платени пакети.\n\n"
        "3. ПРОИЗВОДНИ И МОДИФИКАЦИИ:\n"
        "   Позволяват се промени в кода за подобрения или лична употреба, при условие че:\n"
        "   a) Производното произведение НЕ се продава.\n"
        "   б) Модифицираната версия се поддържа в публично хранилище (отворен код).\n"
        "   в) Кредитите към оригиналния автор се поддържат ясни.\n\n"
        "4. РАЗПРОСТРАНЕНИЕ В МОДУЛИ (MAGISK/KERNELSU):\n"
        "   Използването на този двоичен файл в оптимизационни модули е разрешено и насърчавано, при условие че модулът се разпространява безплатно.\n\n"
        "СОФТУЕРЪТ СЕ ПРЕДОСТАВЯ 'КАКТО Е', БЕЗ ГАРАНЦИЯ ОТ КАКЪВТО И ДА Е ВИД, ИЗРИЧНА ИЛИ ПОДРАЗБИРАНА.\n"
        "ПРИ НИКАКВИ ОБСТОЯТЕЛСТВА АВТОРЪТ НЕ НОСИ ОТГОВОРНОСТ ЗА КАКВИТО И ДА Е ИСКОВЕ, ЩЕТИ ИЛИ ДРУГА ОТГОВОРНОСТ, ПРОИЗТИЧАЩИ ОТ ИЗПОЛЗВАНЕТО НА ТОЗИ СОФТУЕР.\n",

        // Помощ
        "NeonX",
        "Употреба: cat файл | neonx [опции]\n\n",
        "-m [0-11]      Режими на анимация\n",
        "-s [стойност]  Скорост (по подразбиране 0.2)\n",
        "-f [стойност]  Честота (по подразбиране 0.3)\n",
        "-d [стойност]  Продължителност (0: безкрайно)\n",
        "-A [ъгъл]      Ъгъл на градиента (0-360 градуса)\n",
        "-p [стойност]  Фиксирани начални стойности\n",
        "-S             Статичен режим\n",
        "-c [ширина]    Фиксирана ширина на градиента\n",
        "-o [0-1]       Прозрачност на ръбовете\n",
        "-F [стойност]  Кадри в секунда (напр. 60, 90)\n",
        "-L             Режим ред по ред (поток)\n",
        "--preset [име] Зареждане на предварителна настройка (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    Квантовият режим може да намали визуалното качество\n",
        "--spin         Извеждане на сурови ANSI цветови кодове за външни скриптове\n",
        "--lang [език]  Изберете език (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      Лиценз на софтуера\n",
        "-v,--version   Версия на двоичния файл\n",
        "-h,--help      Показване на тази помощ\n",

        // Съобщения за грешки
        "\033[1;31m[NeonX Грешка 400]: Опцията '%s' изисква числова стойност след нея.\033[0m\n",
        "\033[1;31m[NeonX Грешка 400]: Опцията '%s' изисква числова стойност, получено: '%s'\033[0m\n",
        "\033[1;31m[NeonX Грешка 400]: Режимът на анимация (-m) трябва да бъде цяло число.\033[0m\n",
        "\033[1;33m[NeonX Инфо 416]: Режимът на анимация (-m) трябва да бъде между 0 и 11.\033[0m\n",
        "\033[1;31m[NeonX Грешка 400]: Невалидна опция или изолиран аргумент '%s'\033[0m\n",
        "\n\033[1;31m[NeonX Грешка 404]: Не бяха предадени данни на двоичния файл! \033[0m\n",
        "\n\033[1;31m[NeonX Грешка 413]: Файлът е твърде голям. Използвайте -L за потоков режим.\033[0m\n",
        "\033[1;33m[NeonX Инфо 403]: Не може да се провери целостта, ограничена система или променен двоичен файл.\033[0m\n",
        "\033[1;31m[NeonX Грешка 403]: Неуспех при отваряне на собствения изпълним файл за проверка на целостта.\033[0m\n",
        "\033[1;31m[NeonX Грешка 403]: Грешка при четене на файл по време на проверка на целостта.\033[0m\n",
        "\033[1;31m[NeonX Грешка 403]: Недостатъчно памет за проверка на целостта.\033[0m\n",
        "\033[1;31m[NeonX Грешка 403]: Невалиден подпис или неправилен шестнадесетичен формат.\033[0m\n",
        "\033[1;31m[NeonX Грешка 403]: Файлът е твърде малък, за да съдържа подписа за цялост.\033[0m\n",
        "\033[1;31m[NeonX Грешка 400]: Опцията '%s' изисква цяло число.\033[0m\n",
        "\033[1;31m[NeonX Грешка 400]: Опцията '%s' изисква положителна стойност.\033[0m\n",
        "\033[1;31m[NeonX Грешка 400]: Продължителността не може да бъде отрицателна.\033[0m\n",
        "\033[1;31m[NeonX Грешка 206]: Неуспешно разпределяне на памет (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX Инфо 403]: Буферът беше превантивно съкратен до 32 MB. Рендирането може да бъде повредено.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX Предупреждение]: Неуспех при зареждане на потребителския ключ, използва се вграден резервен.\033[0m\n"
    },

    // ---------------- GREGO (8) ----------------
    {
        "Σφάλμα ανοίγματος αρχείου",

        // Έκδοση
        "Αρχικός Δημιουργός: ",
        "Μεταγλωττίστηκε από: ",
        "Κατάσταση: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "Κατάσταση: \033[1;31mMODIFIED\033[0m\n",
        "Κατάσταση: \033[1;33mVERIFY_ERROR\033[0m\n",

        // Άδεια χρήσης
        "ΑΔΕΙΑ ΧΡΗΣΗΣ - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Πνευματικά δικαιώματα (c) 2026 @inrryoff - Αδειοδοτείται υπό ειδικούς όρους NeonX LICENSE\n\n"
        "Με το παρόν, χορηγείται άδεια σε οποιοδήποτε άτομο αποκτά αντίγραφο αυτού του λογισμικού να το χρησιμοποιεί δωρεάν, υπό τις ακόλουθες προϋποθέσεις:\n\n"
        "1. ΑΠΟΔΟΣΗ (ΠΙΣΤΩΣΕΙΣ):\n"
        "   Το όνομα του αρχικού δημιουργού (@inrryoff) και οι ειδοποιήσεις πνευματικών δικαιωμάτων πρέπει να διατηρούνται σε όλα τα αρχεία πηγαίου κώδικα, κεφαλίδες και στην έξοδο έκδοσης του μεταγλωττισμένου δυαδικού αρχείου (π.χ. neonx --version).\n\n"
        "2. ΑΠΑΓΟΡΕΥΣΗ ΕΜΠΟΡΕΥΜΑΤΟΠΟΙΗΣΗΣ:\n"
        "   ΑΠΑΓΟΡΕΥΕΤΑΙ ΑΥΣΤΗΡΑ η πώληση, ενοικίαση ή οποιαδήποτε μορφή εμπορευματοποίησης αυτού του λογισμικού, είτε ως πηγαίος κώδικας είτε ως μεταγλωττισμένο δυαδικό αρχείο, αυτόνομα ή ενσωματωμένο σε πληρωμένα πακέτα.\n\n"
        "3. ΠΑΡΑΓΩΓΑ ΚΑΙ ΤΡΟΠΟΠΟΙΗΣΕΙΣ:\n"
        "   Επιτρέπονται αλλαγές κώδικα για βελτιώσεις ή προσωπική χρήση, υπό την προϋπόθεση ότι:\n"
        "   α) Το παράγωγο έργο ΔΕΝ πωλείται.\n"
        "   β) Η τροποποιημένη έκδοση διατηρείται σε δημόσιο αποθετήριο (Ανοιχτού Κώδικα).\n"
        "   γ) Οι πιστώσεις στον αρχικό δημιουργό διατηρούνται σαφείς.\n\n"
        "4. ΔΙΑΝΟΜΗ ΣΕ ΛΕΙΤΟΥΡΓΙΚΕΣ ΜΟΝΑΔΕΣ (MAGISK/KERNELSU):\n"
        "   Η χρήση αυτού του δυαδικού αρχείου σε μονάδες βελτιστοποίησης επιτρέπεται και ενθαρρύνεται, υπό την προϋπόθεση ότι η μονάδα διανέμεται δωρεάν.\n\n"
        "ΤΟ ΛΟΓΙΣΜΙΚΟ ΠΑΡΕΧΕΤΑΙ 'ΩΣ ΕΧΕΙ', ΧΩΡΙΣ ΚΑΜΙΑ ΕΓΓΥΗΣΗ, ΡΗΤΗ Ή ΣΙΩΠΗΡΗ.\n"
        "ΣΕ ΚΑΜΙΑ ΠΕΡΙΠΤΩΣΗ Ο ΔΗΜΙΟΥΡΓΟΣ ΔΕΝ ΦΕΡΕΙ ΕΥΘΥΝΗ ΓΙΑ ΟΠΟΙΑΔΗΠΟΤΕ ΑΞΙΩΣΗ, ΖΗΜΙΑ Ή ΑΛΛΗ ΕΥΘΥΝΗ ΠΟΥ ΠΡΟΚΥΠΤΕΙ ΑΠΟ ΤΗ ΧΡΗΣΗ ΑΥΤΟΥ ΤΟΥ ΛΟΓΙΣΜΙΚΟΥ.\n",

        // Βοήθεια
        "NeonX",
        "Χρήση: cat αρχείο | neonx [επιλογές]\n\n",
        "-m [0-11]      Λειτουργίες κινούμενων σχεδίων\n",
        "-s [τιμή]     Ταχύτητα (προεπιλογή 0.2)\n",
        "-f [τιμή]     Συχνότητα (προεπιλογή 0.3)\n",
        "-d [τιμή]     Διάρκεια (0: άπειρη)\n",
        "-A [γωνία]    Γωνία διαβάθμισης (0-360 μοίρες)\n",
        "-p [τιμή]     Σταθεροί σπόροι\n",
        "-S             Στατική λειτουργία\n",
        "-c [πλάτος]   Σταθερό πλάτος διαβάθμισης\n",
        "-o [0-1]      Αδιαφάνεια άκρων\n",
        "-F [τιμή]     FPS (π.χ. 60, 90)\n",
        "-L             Λειτουργία γραμμή προς γραμμή (ροή)\n",
        "--preset [όνομα] Φόρτωση προεπιλογής (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    Η κβαντισμένη λειτουργία μπορεί να μειώσει την οπτική ποιότητα\n",
        "--spin         Έξοδος ακατέργαστων κωδίκων χρώματος ANSI για εξωτερικά σενάρια\n",
        "--lang [γλώσσα] Επιλέξτε γλώσσα (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      Άδεια χρήσης λογισμικού\n",
        "-v,--version   Έκδοση δυαδικού αρχείου\n",
        "-h,--help      Εμφάνιση αυτής της βοήθειας\n",

        // Μηνύματα σφάλματος
        "\033[1;31m[NeonX Σφάλμα 400]: Η επιλογή '%s' απαιτεί μια αριθμητική τιμή μετά από αυτήν.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 400]: Η επιλογή '%s' απαιτεί μια αριθμητική τιμή, ελήφθη: '%s'\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 400]: Η λειτουργία κινούμενων σχεδίων (-m) πρέπει να είναι ακέραιος αριθμός.\033[0m\n",
        "\033[1;33m[NeonX Πληροφορία 416]: Η λειτουργία κινούμενων σχεδίων (-m) πρέπει να είναι μεταξύ 0 και 11.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 400]: Μη έγκυρη επιλογή ή μεμονωμένο όρισμα '%s'\033[0m\n",
        "\n\033[1;31m[NeonX Σφάλμα 404]: Δεν μεταβιβάστηκαν δεδομένα στο δυαδικό αρχείο! \033[0m\n",
        "\n\033[1;31m[NeonX Σφάλμα 413]: Το αρχείο είναι πολύ μεγάλο. Χρησιμοποιήστε -L για λειτουργία ροής.\033[0m\n",
        "\033[1;33m[NeonX Πληροφορία 403]: Δεν ήταν δυνατή η επαλήθευση ακεραιότητας, περιορισμένο σύστημα ή παραβιασμένο δυαδικό αρχείο.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 403]: Αποτυχία ανοίγματος του δικού του εκτελέσιμου αρχείου για έλεγχο ακεραιότητας.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 403]: Σφάλμα ανάγνωσης αρχείου κατά τον έλεγχο ακεραιότητας.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 403]: Ανεπαρκής μνήμη για επαλήθευση ακεραιότητας.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 403]: Μη έγκυρη υπογραφή ή εσφαλμένη δεκαεξαδική μορφή.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 403]: Το αρχείο είναι πολύ μικρό για να περιέχει την υπογραφή ακεραιότητας.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 400]: Η επιλογή '%s' απαιτεί έναν ακέραιο αριθμό.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 400]: Η επιλογή '%s' απαιτεί μια θετική τιμή.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 400]: Η διάρκεια δεν μπορεί να είναι αρνητική.\033[0m\n",
        "\033[1;31m[NeonX Σφάλμα 206]: Αδυναμία εκχώρησης μνήμης (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX Πληροφορία 403]: Το buffer περικόπηκε προληπτικά στα 32 MB. Η απόδοση ενδέχεται να καταστραφεί.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX Προειδοποίηση]: Αποτυχία φόρτωσης προσαρμοσμένου κλειδιού, χρήση ενσωματωμένου εφεδρικού.\033[0m\n"
    },

    // ---------------- COREANO (9) ----------------
    {
        "파일 열기 오류",

        // 버전
        "원본 작성자: ",
        "컴파일: %s\n",
        "상태: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "상태: \033[1;31mMODIFIED\033[0m\n",
        "상태: \033[1;33mVERIFY_ERROR\033[0m\n",

        // 라이선스
        "사용 라이선스 - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "저작권 (c) 2026 @inrryoff - NeonX LICENSE 특수 조건에 따라 라이선스됨\n\n"
        "이 소프트웨어의 사본을 얻는 모든 사람은 다음 조건에 따라 무료로 사용할 수 있는 권한이 부여됩니다.\n\n"
        "1. 저작자 표시 (크레딧):\n"
        "   원본 작성자의 이름(@inrryoff)과 저작권 표시는 모든 소스 코드 파일, 헤더 및 컴파일된 바이너리의 버전 출력(예: neonx --version)에 유지되어야 합니다.\n\n"
        "2. 상업화 금지:\n"
        "   소스 코드 또는 컴파일된 바이너리 형태를 막론하고, 이 소프트웨어를 판매, 임대 또는 상업화하는 것은 엄격히 금지됩니다.\n\n"
        "3. 파생 및 수정:\n"
        "   개선 또는 개인적 사용을 위한 코드 변경이 허용됩니다. 단, 다음 조건을 충족해야 합니다:\n"
        "   a) 파생 저작물을 판매하지 않습니다.\n"
        "   b) 수정된 버전은 공개 저장소(오픈 소스)에 유지됩니다.\n"
        "   c) 원본 작성자에 대한 크레딧이 명확하게 유지됩니다.\n\n"
        "4. 모듈 배포 (MAGISK/KERNELSU):\n"
        "   이 바이너리를 최적화 모듈에서 사용하는 것은 모듈이 무료로 배포되는 한 허용되며 권장됩니다.\n\n"
        "이 소프트웨어는 '있는 그대로' 제공되며, 명시적이거나 묵시적인 어떠한 보증도 없습니다.\n"
        "어떠한 경우에도 저자는 이 소프트웨어 사용으로 인해 발생하는 청구, 손해 또는 기타 책임에 대해 책임을 지지 않습니다.\n",

        // 도움말
        "NeonX",
        "사용법: cat 파일 | neonx [옵션]\n\n",
        "-m [0-11]      애니메이션 모드\n",
        "-s [값]        속도 (기본값 0.2)\n",
        "-f [값]        주파수 (기본값 0.3)\n",
        "-d [값]        지속 시간 (0: 무한)\n",
        "-A [각도]      그라데이션 각도 (0-360도)\n",
        "-p [값]        고정 시드\n",
        "-S             정적 모드\n",
        "-c [너비]      고정 그라데이션 너비\n",
        "-o [0-1]       가장자리 불투명도\n",
        "-F [값]        FPS (예: 60, 90)\n",
        "-L             줄 단위 모드 (스트림)\n",
        "--preset [이름] 프리셋 로드 (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    양자화 모드는 시각적 품질을 저하시킬 수 있습니다\n",
        "--spin         외부 스크립트용 원시 ANSI 색상 코드 출력\n",
        "--lang [언어]  언어 선택 (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      소프트웨어 라이선스\n",
        "-v,--version   바이너리 버전\n",
        "-h,--help      이 도움말 표시\n",

        // 오류 메시지
        "\033[1;31m[NeonX 오류 400]: '%s' 옵션 뒤에는 숫자 값이 필요합니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 400]: '%s' 옵션에는 숫자 값이 필요합니다. 받은 값: '%s'\033[0m\n",
        "\033[1;31m[NeonX 오류 400]: 애니메이션 모드 (-m)는 정수여야 합니다.\033[0m\n",
        "\033[1;33m[NeonX 정보 416]: 애니메이션 모드 (-m)는 0과 11 사이여야 합니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 400]: 잘못된 옵션이나 분리된 인수 '%s'\033[0m\n",
        "\n\033[1;31m[NeonX 오류 404]: 바이너리에 데이터가 전달되지 않았습니다! \033[0m\n",
        "\n\033[1;31m[NeonX 오류 413]: 파일이 너무 큽니다. 스트리밍 모드에는 -L을 사용하세요.\033[0m\n",
        "\033[1;33m[NeonX 정보 403]: 무결성을 확인할 수 없습니다. 시스템이 제한되었거나 바이너리가 변조되었습니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 403]: 무결성 검사를 위해 자신의 실행 파일을 열지 못했습니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 403]: 무결성 검사 중 파일 읽기 오류가 발생했습니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 403]: 무결성을 검증하기에 메모리가 부족합니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 403]: 서명이 잘못되었거나 16진수 형식이 올바르지 않습니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 403]: 파일이 너무 작아 무결성 서명을 포함할 수 없습니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 400]: '%s' 옵션에는 정수가 필요합니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 400]: '%s' 옵션에는 양수 값이 필요합니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 400]: 지속 시간은 음수가 될 수 없습니다.\033[0m\n",
        "\033[1;31m[NeonX 오류 206]: 메모리를 할당할 수 없습니다 (wcsdup).\033[0m\n",
        "\033[1;33m[NeonX 정보 403]: 버퍼가 예방적으로 32MB로 잘렸습니다. 렌더링이 손상될 수 있습니다.\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX 경고]: 사용자 지정 키를 로드하지 못했습니다. 내장된 대체 키를 사용합니다.\033[0m\n"
    },

    // ---------------- HINDI (10) ----------------
    {
        "फ़ाइल खोलने में त्रुटि",

        // संस्करण
        "मूल निर्माता: ",
        "द्वारा संकलित: ",
        "स्थिति: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "स्थिति: \033[1;31mMODIFIED\033[0m\n",
        "स्थिति: \033[1;33mVERIFY_ERROR\033[0m\n",

        // लाइसेंस
        "उपयोग लाइसेंस - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "कॉपीराइट (c) 2026 @inrryoff - NeonX LICENSE विशेष शर्तों के तहत लाइसेंसीकृत\n\n"
        "इस सॉफ़्टवेयर की एक प्रति प्राप्त करने वाले किसी भी व्यक्ति को, निम्नलिखित शर्तों के अधीन, इसे निःशुल्क उपयोग करने की अनुमति दी जाती है:\n\n"
        "1. श्रेय (क्रेडिट):\n"
        "   मूल लेखक का नाम (@inrryoff) और कॉपीराइट सूचनाएँ सभी स्रोत कोड फ़ाइलों, हेडरों और संकलित बाइनरी के संस्करण आउटपुट (जैसे neonx --version) में बनाए रखी जानी चाहिए।\n\n"
        "2. व्यावसायीकरण का निषेध:\n"
        "   इस सॉफ़्टवेयर को बेचना, किराए पर देना या किसी भी रूप में व्यावसायीकरण करना सख्त वर्जित है, चाहे वह स्रोत कोड हो या संकलित बाइनरी, अकेले या भुगतान पैकेजों में एकीकृत।\n\n"
        "3. व्युत्पन्न और संशोधन:\n"
        "   सुधार या व्यक्तिगत उपयोग के लिए कोड बदलने की अनुमति है, बशर्ते कि:\n"
        "   a) व्युत्पन्न कार्य बेचा न जाए।\n"
        "   b) संशोधित संस्करण को सार्वजनिक भंडार (ओपन सोर्स) में रखा जाए।\n"
        "   c) मूल लेखक को स्पष्ट रूप से श्रेय दिया जाए।\n\n"
        "4. मॉड्यूल में वितरण (MAGISK/KERNELSU):\n"
        "   इस बाइनरी को ऑप्टिमाइज़ेशन मॉड्यूल में उपयोग करने की अनुमति है और इसे प्रोत्साहित किया जाता है, बशर्ते मॉड्यूल मुफ्त में वितरित किया जाए।\n\n"
        "सॉफ़्टवेयर 'जैसा है' प्रदान किया जाता है, बिना किसी प्रकार की वारंटी, व्यक्त या निहित, के।\n"
        "किसी भी स्थिति में, इस सॉफ़्टवेयर के उपयोग से उत्पन्न किसी भी दावे, क्षति या अन्य दायित्व के लिए लेखक उत्तरदायी नहीं होगा।\n",

        // सहायता
        "NeonX",
        "उपयोग: cat फ़ाइल | neonx [विकल्प]\n\n",
        "-m [0-11]      एनिमेशन मोड\n",
        "-s [मान]      गति (डिफ़ॉल्ट 0.2)\n",
        "-f [मान]      आवृत्ति (डिफ़ॉल्ट 0.3)\n",
        "-d [मान]      अवधि (0: अनंत)\n",
        "-A [कोण]      ग्रेडिएंट कोण (0-360 डिग्री)\n",
        "-p [मान]      फिक्स्ड सीड्स\n",
        "-S             स्थैतिक मोड\n",
        "-c [चौड़ाई]   फिक्स्ड ग्रेडिएंट चौड़ाई\n",
        "-o [0-1]       किनारे की अपारदर्शिता\n",
        "-F [मान]      FPS (उदा. 60, 90)\n",
        "-L             लाइन-बाय-लाइन मोड (स्ट्रीम)\n",
        "--preset [नाम] प्रीसेट लोड करें (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    क्वांटाइज्ड मोड दृश्य गुणवत्ता कम कर सकता है\n",
        "--spin         बाहरी स्क्रिप्ट के लिए कच्चे ANSI रंग कोड आउटपुट करें\n",
        "--lang [भाषा] भाषा चुनें (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      सॉफ़्टवेयर लाइसेंस\n",
        "-v,--version   बाइनरी संस्करण\n",
        "-h,--help      यह सहायता दिखाएँ\n",

        // त्रुटि संदेश
        "\033[1;31m[NeonX त्रुटि 400]: विकल्प '%s' के बाद एक संख्यात्मक मान की आवश्यकता है।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 400]: विकल्प '%s' के लिए एक संख्यात्मक मान की आवश्यकता है, प्राप्त: '%s'\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 400]: एनिमेशन मोड (-m) एक पूर्णांक होना चाहिए।\033[0m\n",
        "\033[1;33m[NeonX जानकारी 416]: एनिमेशन मोड (-m) 0 और 11 के बीच होना चाहिए।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 400]: अमान्य विकल्प या अलग तर्क '%s'\033[0m\n",
        "\n\033[1;31m[NeonX त्रुटि 404]: बाइनरी को कोई डेटा पास नहीं किया गया! \033[0m\n",
        "\n\033[1;31m[NeonX त्रुटि 413]: फ़ाइल बहुत बड़ी है। स्ट्रीमिंग मोड के लिए -L का उपयोग करें।\033[0m\n",
        "\033[1;33m[NeonX जानकारी 403]: अखंडता की जांच नहीं कर सकता, प्रतिबंधित प्रणाली या छेड़छाड़ वाली बाइनरी।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 403]: अखंडता जांच के लिए अपनी निष्पादन योग्य फ़ाइल खोलने में विफल।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 403]: अखंडता जांच के दौरान फ़ाइल पढ़ने की त्रुटि।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 403]: अखंडता सत्यापित करने के लिए अपर्याप्त मेमोरी।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 403]: अमान्य हस्ताक्षर या गलत हेक्साडेसिमल प्रारूप।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 403]: फ़ाइल अखंडता हस्ताक्षर रखने के लिए बहुत छोटी है।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 400]: विकल्प '%s' के लिए एक पूर्णांक की आवश्यकता है।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 400]: विकल्प '%s' के लिए एक सकारात्मक मान की आवश्यकता है।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 400]: अवधि ऋणात्मक नहीं हो सकती।\033[0m\n",
        "\033[1;31m[NeonX त्रुटि 206]: मेमोरी आवंटित नहीं कर सका (wcsdup)।\033[0m\n",
        "\033[1;33m[NeonX जानकारी 403]: बफर को निवारक रूप से 32MB तक छोटा किया गया। रेंडरिंग क्षतिग्रस्त हो सकती है।\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX चेतावनी]: कस्टम कुंजी लोड करने में विफल, अंतर्निहित फ़ॉलबैक का उपयोग कर रहा है।\033[0m\n"
    },

    // ---------------- TAILANDES (11) ----------------
    {
        "เกิดข้อผิดพลาดในการเปิดไฟล์",

        // รุ่น
        "ผู้สร้างดั้งเดิม: ",
        "คอมไพล์โดย: ",
        "สถานะ: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "สถานะ: \033[1;31mMODIFIED\033[0m\n",
        "สถานะ: \033[1;33mVERIFY_ERROR\033[0m\n",

        // สัญญาอนุญาต
        "สัญญาอนุญาตการใช้งาน - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "ลิขสิทธิ์ (c) 2026 @inrryoff - ได้รับอนุญาตภายใต้เงื่อนไขพิเศษ NeonX LICENSE\n\n"
        "ด้วยเอกสารนี้ อนุญาตให้บุคคลใดก็ตามที่ได้รับสำเนาของซอฟต์แวร์นี้ใช้งานฟรี โดยอยู่ภายใต้เงื่อนไขต่อไปนี้:\n\n"
        "1. การแสดงที่มา (เครดิต):\n"
        "   ชื่อผู้สร้างดั้งเดิม (@inrryoff) และประกาศลิขสิทธิ์จะต้องถูกเก็บไว้ในไฟล์ซอร์สโค้ดทั้งหมด, ส่วนหัว และในผลลัพธ์เวอร์ชันของไบนารีที่คอมไพล์แล้ว (เช่น neonx --version)\n\n"
        "2. ห้ามนำไปใช้ในเชิงพาณิชย์:\n"
        "   ห้ามจำหน่าย ให้เช่า หรือนำซอฟต์แวร์นี้ไปใช้ในเชิงพาณิชย์ในรูปแบบใดๆ ไม่ว่าจะเป็นซอร์สโค้ดหรือไบนารีที่คอมไพล์แล้ว โดยเดี่ยวหรือรวมในแพ็คเกจที่ต้องชำระเงิน\n\n"
        "3. งานดัดแปลงและการแก้ไข:\n"
        "   อนุญาตให้แก้ไขโค้ดเพื่อการปรับปรุงหรือใช้งานส่วนตัวได้ โดยต้อง:\n"
        "   ก) งานดัดแปลงต้องไม่ถูกขาย\n"
        "   ข) เวอร์ชันที่แก้ไขแล้วต้องถูกเก็บไว้ในคลังสาธารณะ (โอเพนซอร์ส)\n"
        "   ค) เครดิตต่อผู้สร้างดั้งเดิมต้องถูกเก็บไว้อย่างชัดเจน\n\n"
        "4. การแจกจ่ายในโมดูล (MAGISK/KERNELSU):\n"
        "   อนุญาตและสนับสนุนให้ใช้ไบนารีนี้ในโมดูลปรับแต่งประสิทธิภาพ โดยมีเงื่อนไขว่าโมดูลนั้นต้องแจกจ่ายฟรี\n\n"
        "ซอฟต์แวร์นี้ถูกจัดเตรียม 'ตามที่เป็น' โดยไม่มีการรับประกันใดๆ ไม่ว่าโดยชัดแจ้งหรือโดยนัย\n"
        "ไม่ว่าในกรณีใด ผู้สร้างจะไม่รับผิดชอบต่อการเรียกร้อง ความเสียหาย หรือความรับผิดอื่นใดที่เกิดจากการใช้ซอฟต์แวร์นี้\n",

        // ความช่วยเหลือ
        "NeonX",
        "การใช้งาน: cat ไฟล์ | neonx [ตัวเลือก]\n\n",
        "-m [0-11]      โหมดแอนิเมชัน\n",
        "-s [ค่า]       ความเร็ว (ค่าเริ่มต้น 0.2)\n",
        "-f [ค่า]       ความถี่ (ค่าเริ่มต้น 0.3)\n",
        "-d [ค่า]       ระยะเวลา (0: ไม่จำกัด)\n",
        "-A [มุม]       มุมเกรเดียนต์ (0-360 องศา)\n",
        "-p [ค่า]       ค่าเริ่มต้นคงที่\n",
        "-S             โหมดคงที่\n",
        "-c [ความกว้าง] ความกว้างเกรเดียนต์คงที่\n",
        "-o [0-1]       ความทึบของขอบ\n",
        "-F [ค่า]       FPS (เช่น 60, 90)\n",
        "-L             โหมดทีละบรรทัด (สตรีม)\n",
        "--preset [ชื่อ] โหลดพรีเซ็ต (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    โหมดควอนไทซ์อาจลดคุณภาพของภาพ\n",
        "--spin         ส่งออกรหัสสี ANSI ดิบสำหรับสคริปต์ภายนอก\n",
        "--lang [ภาษา] เลือกภาษา (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      สัญญาอนุญาตซอฟต์แวร์\n",
        "-v,--version   เวอร์ชันของไบนารี\n",
        "-h,--help      แสดงความช่วยเหลือนี้\n",

        // ข้อความแสดงข้อผิดพลาด
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: ตัวเลือก '%s' ต้องการค่าตัวเลขตามหลัง\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: ตัวเลือก '%s' ต้องการค่าตัวเลข, ได้รับ: '%s'\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: โหมดแอนิเมชัน (-m) ต้องเป็นจำนวนเต็ม\033[0m\n",
        "\033[1;33m[NeonX ข้อมูล 416]: โหมดแอนิเมชัน (-m) ต้องอยู่ระหว่าง 0 ถึง 11\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: ตัวเลือกไม่ถูกต้องหรืออาร์กิวเมนต์หลุด '%s'\033[0m\n",
        "\n\033[1;31m[NeonX ข้อผิดพลาด 404]: ไม่มีการส่งข้อมูลไปยังไบนารี! \033[0m\n",
        "\n\033[1;31m[NeonX ข้อผิดพลาด 413]: ไฟล์ใหญ่เกินไป ใช้ -L สำหรับโหมดสตรีม\033[0m\n",
        "\033[1;33m[NeonX ข้อมูล 403]: ไม่สามารถตรวจสอบความสมบูรณ์ได้ ระบบถูกจำกัดหรือไบนารีถูกแก้ไข\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 403]: ไม่สามารถเปิดไฟล์ปฏิบัติการของตัวเองเพื่อตรวจสอบความสมบูรณ์\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 403]: ข้อผิดพลาดในการอ่านไฟล์ระหว่างการตรวจสอบความสมบูรณ์\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 403]: หน่วยความจำไม่เพียงพอที่จะตรวจสอบความสมบูรณ์\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 403]: ลายเซ็นไม่ถูกต้องหรือรูปแบบเลขฐานสิบหกไม่ถูกต้อง\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 403]: ไฟล์เล็กเกินไปที่จะบรรจุลายเซ็นความสมบูรณ์\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: ตัวเลือก '%s' ต้องการจำนวนเต็ม\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: ตัวเลือก '%s' ต้องการค่าบวก\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 400]: ระยะเวลาไม่สามารถเป็นลบได้\033[0m\n",
        "\033[1;31m[NeonX ข้อผิดพลาด 206]: ไม่สามารถจัดสรรหน่วยความจำ (wcsdup)\033[0m\n",
        "\033[1;33m[NeonX ข้อมูล 403]: บัฟเฟอร์ถูกตัดเหลือ 32MB เพื่อป้องกันไว้ก่อน การเรนเดอร์อาจเสียหาย\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX คำเตือน]: โหลดคีย์แบบกำหนดเองล้มเหลว กำลังใช้คีย์สำรองในตัว\033[0m\n"
    },

    // ---------------- KHMER (12) ----------------
    {
        "កំហុសក្នុងការបើកឯកសារ",

        // កំណែ
        "អ្នកបង្កើតដើម: ",
        "ចងក្រងដោយ: ",
        "ស្ថានភាព: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "ស្ថានភាព: \033[1;31mMODIFIED\033[0m\n",
        "ស្ថានភាព: \033[1;33mVERIFY_ERROR\033[0m\n",

        // អាជ្ញាប័ណ្ណ
        "អាជ្ញាប័ណ្ណប្រើប្រាស់ - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "រក្សាសិទ្ធិ (c) 2026 @inrryoff - បានផ្តល់អាជ្ញាប័ណ្ណក្រោមលក្ខខណ្ឌពិសេស NeonX LICENSE\n\n"
        "ដោយឯកសារនេះ បុគ្គលណាដែលទទួលបានច្បាប់ចម្លងនៃកម្មវិធីនេះ ត្រូវបានផ្តល់សិទ្ធិប្រើប្រាស់ដោយឥតគិតថ្លៃ ក្រោមលក្ខខណ្ឌដូចខាងក្រោម:\n\n"
        "1. ការដកស្រង់ប្រភព (ក្រេឌីត):\n"
        "   ត្រូវតែរក្សាឈ្មោះអ្នកបង្កើតដើម (@inrryoff) និងសេចក្តីជូនដំណឹងរក្សាសិទ្ធិនៅក្នុងឯកសារកូដប្រភពទាំងអស់ ឯកសារបឋមកថា និងលទ្ធផលកំណែនៃឯកសារគោលដែលបានចងក្រង (ឧ. neonx --version)។\n\n"
        "2. ការហាមឃាត់ការធ្វើពាណិជ្ជកម្ម:\n"
        "   ត្រូវហាមឃាត់យ៉ាងតឹងរ៉ឹងក្នុងការលក់ ជួល ឬធ្វើពាណិជ្ជកម្មកម្មវិធីនេះក្នុងទម្រង់ណាមួយ មិនថាជាកូដប្រភព ឬឯកសារគោលដែលបានចងក្រង ដោយឯករាជ្យ ឬរួមបញ្ចូលក្នុងកញ្ចប់បង់ប្រាក់។\n\n"
        "3. ការដកស្រង់បន្ត និងការកែប្រែ:\n"
        "   ការកែប្រែកូដត្រូវបានអនុញ្ញាតសម្រាប់ការកែលម្អ ឬប្រើប្រាស់ផ្ទាល់ខ្លួន ដោយមានលក្ខខណ្ឌថា:\n"
        "   ក) កុំលក់ស្នាដៃដកស្រង់បន្ត។\n"
        "   ខ) រក្សាកំណែដែលបានកែប្រែនៅក្នុងឃ្លាំងសាធារណៈ (ប្រភពបើកចំហ)។\n"
        "   គ) ត្រូវរក្សាក្រេឌីតចំពោះអ្នកបង្កើតដើមឱ្យបានច្បាស់លាស់។\n\n"
        "4. ការចែកចាយក្នុងម៉ូឌុល (MAGISK/KERNELSU):\n"
        "   ការប្រើប្រាស់ឯកសារគោលនេះក្នុងម៉ូឌុលបង្កើនប្រសិទ្ធភាពត្រូវបានអនុញ្ញាត និងលើកទឹកចិត្ត ដោយមានលក្ខខណ្ឌថាម៉ូឌុលត្រូវបានចែកចាយដោយឥតគិតថ្លៃ។\n\n"
        "កម្មវិធីនេះត្រូវបានផ្តល់ជូន 'ដូចដែលវាមាន' ដោយគ្មានការធានាណាមួយ ទាំងបង្ហាញឱ្យឃើញ ឬបង្កប់ន័យ។\n"
        "ក្នុងករណីណាក៏ដោយ អ្នកបង្កើតនឹងមិនទទួលខុសត្រូវចំពោះការទាមទារ ការខូចខាត ឬការទទួលខុសត្រូវផ្សេងទៀតដែលកើតចេញពីការប្រើប្រាស់កម្មវិធីនេះឡើយ។\n",

        // ជំនួយ
        "NeonX",
        "ការប្រើប្រាស់: cat ឯកសារ | neonx [ជម្រើស]\n\n",
        "-m [0-11]      របៀបចលនា\n",
        "-s [តម្លៃ]     ល្បឿន (លំនាំដើម 0.2)\n",
        "-f [តម្លៃ]     ប្រេកង់ (លំនាំដើម 0.3)\n",
        "-d [តម្លៃ]     រយៈពេល (0: គ្មានកំណត់)\n",
        "-A [មុំ]       មុំជម្រាលពណ៌ (0-360 ដឺក្រេ)\n",
        "-p [តម្លៃ]     គ្រាប់ពូជថេរ\n",
        "-S             របៀបឋិតិវន្ត\n",
        "-c [ទទឹង]     ទទឹងជម្រាលពណ៌ថេរ\n",
        "-o [0-1]       ភាពស្រអាប់គែម\n",
        "-F [តម្លៃ]     FPS (ឧ. 60, 90)\n",
        "-L             របៀបបន្ទាត់ម្តង (ស្ទ្រីម)\n",
        "--preset [ឈ្មោះ] ផ្ទុកបុរេកំណត់ (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    របៀបបរិមាណអាចបន្ថយគុណភាពមើលឃើញ\n",
        "--spin         បញ្ចេញកូដពណ៌ ANSI ឆៅសម្រាប់ស្គ្រីបខាងក្រៅ\n",
        "--lang [ភាសា] ជ្រើសភាសា (pt, en, es, zh, ja, ar, ru, bg, el, ko, hi, th, km)\n",
        "--license      អាជ្ញាប័ណ្ណកម្មវិធី\n",
        "-v,--version   កំណែឯកសារគោល\n",
        "-h,--help      បង្ហាញជំនួយនេះ\n",

        // សារកំហុស
        "\033[1;31m[NeonX កំហុស 400]: ជម្រើស '%s' ត្រូវការតម្លៃលេខបន្ទាប់ពីវា។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 400]: ជម្រើស '%s' ត្រូវការតម្លៃលេខ, ទទួលបាន: '%s'\033[0m\n",
        "\033[1;31m[NeonX កំហុស 400]: របៀបចលនា (-m) ត្រូវតែជាចំនួនគត់។\033[0m\n",
        "\033[1;33m[NeonX ព័ត៌មាន 416]: របៀបចលនា (-m) ត្រូវតែនៅចន្លោះ 0 និង 11។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 400]: ជម្រើសមិនត្រឹមត្រូវ ឬអាគុយម៉ង់រលុង '%s'\033[0m\n",
        "\n\033[1;31m[NeonX កំហុស 404]: គ្មានទិន្នន័យត្រូវបានបញ្ជូនទៅឯកសារគោល! \033[0m\n",
        "\n\033[1;31m[NeonX កំហុស 413]: ឯកសារធំពេក។ ប្រើ -L សម្រាប់របៀបស្ទ្រីម។\033[0m\n",
        "\033[1;33m[NeonX ព័ត៌មាន 403]: មិនអាចពិនិត្យសុចរិតភាពបានទេ ប្រព័ន្ធត្រូវបានកំណត់ ឬឯកសារគោលត្រូវបានកែប្រែ។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 403]: បរាជ័យក្នុងការបើកឯកសារដែលអាចប្រតិបត្តិបានរបស់ខ្លួនសម្រាប់ពិនិត្យសុចរិតភាព។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 403]: កំហុសក្នុងការអានឯកសារកំឡុងពេលពិនិត្យសុចរិតភាព។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 403]: អង្គចងចាំមិនគ្រប់គ្រាន់ដើម្បីផ្ទៀងផ្ទាត់សុចរិតភាព។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 403]: ហត្ថលេខាមិនត្រឹមត្រូវ ឬទម្រង់គោលដប់ប្រាំមួយមិនត្រឹមត្រូវ។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 403]: ឯកសារតូចពេកមិនអាចផ្ទុកហត្ថលេខាសុចរិតភាព។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 400]: ជម្រើស '%s' ត្រូវការចំនួនគត់។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 400]: ជម្រើស '%s' ត្រូវការតម្លៃវិជ្ជមាន។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 400]: រយៈពេលមិនអាចជាចំនួនអវិជ្ជមាន។\033[0m\n",
        "\033[1;31m[NeonX កំហុស 206]: មិនអាចបែងចែកអង្គចងចាំ (wcsdup)។\033[0m\n",
        "\033[1;33m[NeonX ព័ត៌មាន 403]: សតិបណ្ដោះអាសន្នត្រូវបានកាត់ជា 32MB ដើម្បីបង្ការ។ ការបង្ហាញអាចខូច។\033[0m\n",
        "\033[1;32mOK\033[0m\n",
        "\033[1;31mFAIL\033[0m\n",
        "\033[1;33m[NeonX ការព្រមាន]: បរាជ័យក្នុងការផ្ទុកសោផ្ទាល់ខ្លួន កំពុងប្រើសោបម្រុងដែលភ្ជាប់មកជាមួយ។\033[0m\n"
    },
};

#ifdef _WIN32
const char* msgs_detect_windows_locale(void) {
    LANGID langId = GetUserDefaultUILanguage();

    wchar_t wlocaleName[LOCALE_NAME_MAX_LENGTH];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, wlocaleName, LOCALE_NAME_MAX_LENGTH) == 0) {
        return "en";
    }

    static char localeName[LOCALE_NAME_MAX_LENGTH];
    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, localeName, LOCALE_NAME_MAX_LENGTH, wlocaleName, _TRUNCATE);

    if (strlen(localeName) >= 2) {
        localeName[2] = '\0';
    }
    return localeName;
}
#endif
void msgs_init(void) {
#ifdef _WIN32
    const char *lang = msgs_detect_windows_locale();
#else
    const char *lang = getenv("LANG");
#endif
    int idx = lookup_language(lang);
    idioma_atual = (idx >= 0) ? idx : 1;  // fallback para inglês (1)
}

void msgs_set_language(const char *lang_code) {
    int idx = lookup_language(lang_code);
    idioma_atual = (idx >= 0) ? idx : 1;
}

const char* get_msg(enum Mensagem id) {
    return mensagens[idioma_atual][id];
}