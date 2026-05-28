// ==================== msgs.c ====================
// Desativa avisos chatos e irrelevantes de segurança em compiladores da Microsoft (Visual Studio)
#define _CRT_SECURE_NO_WARNINGS

#include "msgs.h"
#include <stdlib.h>
#include <string.h>

// Variável estática (static significa que este valor é invisível para os outros arquivos .c,
// ele existe exclusivamente dentro da memória do msgs.c).
// Guarda o índice do idioma ativo. Começa em 0 (Português).
static int idioma_atual = 0;

// 'mensagens' é uma Matriz Bidimensional (um Array de Arrays).
// Pense nela como uma planilha do Excel:
// - As LINHAS (primeiro colchete [4]) representam os 4 idiomas disponíveis.
// - As COLUNAS (segundo colchete [MSG_TOTAL]) representam a frase que queremos pegar.
static const char *mensagens[4][MSG_TOTAL] = {
    // ---------------- PORTUGUÊS (0) ----------------
    {
        "Erro ao abrir arquivo",

        // Versão
        "Criador Original: %s\n",
        "Compilado por: %s\n",
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
        "OK\n",
        "FAIL\n",
        "\033[1;33m[NeonX Aviso]: Falha ao carregar chave customizada, usando chave embutida.\033[0m\n"
    },

    // ---------------- ENGLISH (1) ----------------
    {
        "\033[1;31mError opening file\033[0m",

        // Versão
        "Original Creator: %s\n",
        "Compiled by: %s\n",
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
        "OK\n",
        "FAIL\n",
        "\033[1;33m[NeonX Warning]: Failed to load custom key, using built-in fallback.\033[0m\n"
    },

    // ---------------- ESPANHOL (2) ----------------
    {
        "\033[1;31mError al abrir el archivo\033[0m",

        // Versão
        "Creador Original: %s\n",
        "Compilado por: %s\n",
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
        "NeonX v%s | Core por: %s | Build por: %s\n\n",
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
        "OK\n",
        "FAIL\n",
        "\033[1;33m[NeonX Aviso]: Error ao cargar la clave personalizada, usando fallback.\033[0m\n"
    },

    // ---------------- CHINÊS (3) ----------------
    {
        "\033[1;31m打开文件失败\033[0m",

        // Versão
        "原作者: %s\n",
        "编译者: %s\n",
        "状态: \033[1;32mOFFICIAL_BY_INRRYOFF\033[0m\n",
        "状态: \033[1;31mMODIFIED\033[0m\n",
        "状态: \033[1;33mVERIFY_ERROR\033[0m\n",

        // Licença
        "使用许可 - NEONX (C - VERSION)\n"
        "-----------------------------------------------------------------\n"
        "Copyright (c) 2026 @inrryoff - 根据 NeonX LICENSE 特别条件授权\n\n"
        "特此授权任何获得本软件副本的人员免费使用本软件，但须满足以下条件：\n\n"
        "1. 署名（鸣谢）：\n"
        "   原作者姓名（@inrryoff）和版权声明必须保留在所有源代码文件、\n"
        "   头文件以及编译后的二进制版本输出中（例如：neonx --version）。\n\n"
        "2. 禁止商业化：\n"
        "   严禁以任何形式销售、出租 or 商业化本软件（无论是源代码还是编译后的二进制文件），\n"
        "   无论其是独立运行还是集成在付费包中。\n\n"
        "3. 衍生与修改：\n"
        "   允许出于改进或个人用途修改代码，前提是：\n"
        "   a) 衍生作品不得销售。\n"
        "   b) 修改后的版本必须保存在公共仓库中（开源）。\n"
        "   c) 必须清晰保留对原作者的署名。\n\n"
        "4. 模块分发 (MAGISK/KERNELSU)：\n"
        "   允许并鼓励在优化模块中使用此二进制文件，前提是该模块免费分发。\n\n"
        "本软件按“原样”提供，不提供任何形式的明示或暗示保证。在任何情况下，\n"
        "作者均不对因使用本软件而产生的任何索赔、损害或其他责任负责。\n",

        // Ajuda
        "NeonX v%s | 核心: %s | 构建: %s\n\n",
        "用法: cat 文件 | neonx [选项]\n\n",
        "-m [0-11]      动画模式\n",
        "-s [数值]      速度（默认 0.2）\n",
        "-f [数值]      频率（默认 0.3）\n",
        "-d [数值]      时长（0: 无限）\n",
        "-A [角度]      渐变角度（0-360 度）\n",
        "-p [数值]      固定种子\n",
        "-S             静态模式\n",
        "-c [宽度]      固定渐变宽度\n",
        "-o [0-1]       边缘透明度\n",
        "-F [数值]      FPS（例如：60, 90)\n",
        "-L             逐行模式 (stream)\n",
        "--preset [名称] 加载预设 (cyberpunk, retro, matrix, sunset)\n",
        "--quantized    量化模式会降低视觉质量\n",
        "--spin         为外部脚本生成纯 ANSI 颜色代码\n",
        "--lang [语言]  选择语言 (pt, en, es, zh)\n",
        "--license      软件许可证\n",
        "-v,--version   二进制版本\n",
        "-h,--help      显示此帮助信息\n",

        // Erros
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 后面需要一个数值。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 需要一个数值，收到: '%s'\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 动画模式 (-m) 必须是整数。\033[0m\n",
        "\033[1;33m[NeonX 信息 416]: 动画模式 (-m) 必须在 0 到 11 之间。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 无效选项或孤立参数 '%s'\033[0m\n",
        "\n\033[1;31m[NeonX 错误 404]: 没有数据传递给二进制文件！\033[0m\n",
        "\n\033[1;31m[NeonX 错误 413]: 文件太大。请使用 -L 进入流媒体模式。\033[0m\n",
        "\033[1;33m[NeonX 信息 403]: 无法检查完整性，系统受限或二进制文件已被篡改。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 无法打开 exec 文件以进行完整性检查。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 完整性检查期间读取文件错误。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 内存不足，无法验证完整性。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 签名无效或十六进制格式不正确。\033[0m\n",
        "\033[1;31m[NeonX 错误 403]: 文件太小，无法包含完整性签名。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 需要一个整数。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 选项 '%s' 需要一个正值。\033[0m\n",
        "\033[1;31m[NeonX 错误 400]: 持续时间不能为负数。\033[0m\n",
        "\033[1;31m[NeonX 错误 206]: 无法分配内存 (wcsdup)。\033[0m\n",
        "OK\n",
        "FAIL\n",
        "\033[1;33m[NeonX 警告]: 无法加载自定义密钥，正在使用内置备用密钥。\033[0m\n"
    }
};

/**
 * Nome da função: msgs_init
 * O que faz: Configura a linguagem das mensagens baseando-se nas configurações de localidade do sistema.
 * Como funciona:
 * 1. Usa a função padrão C 'getenv("LANG")' para perguntar ao Sistema Operacional qual é o idioma atual.
 * (O SO geralmente retorna strings como "pt_BR.UTF-8", "en_US.UTF-8", etc.)
 * 2. Verifica se a resposta não foi nula (ou seja, se a variável LANG existe na máquina).
 * 3. Usa 'strncmp' (String Compare para os primeiros 'N' caracteres) para ler só as duas primeiras letras.
 * 4. Atribui o número equivalente ao idioma detectado na matriz 'mensagens'.
 * Parâmetros: Nenhum.
 * Retorno: Nenhum (void). Apenas altera a variável global 'idioma_atual'.
 * Onde é usada: Chamada apenas uma vez na inicialização, logo no começo do main().
 * Observações: Se não identificar o idioma ou a variável de ambiente não existir,
 * o sistema força '0' (Português) ou '1' (Inglês) como fallback de segurança,
 * garantindo que o programa nunca tente acessar uma linha inexistente na matriz.
 */
void msgs_init(void)
{
    const char *lang = getenv("LANG"); // Consulta o Sistema Operacional

    if (lang != NULL) {
        // strncmp retorna 0 quando as duas strings são iguais!
        if (strncmp(lang, "pt", 2) == 0) {
            idioma_atual = 0; // Português
        } else if (strncmp(lang, "es", 2) == 0) {
            idioma_atual = 2; // Espanhol
        } else if (strncmp(lang, "zh", 2) == 0) {
            idioma_atual = 3; // Chinês
        } else {
            idioma_atual = 1; // Fallback: se for francês, alemão, ou até inglês, usa Inglês.
        }
    } else {
        idioma_atual = 0; // Fallback extremo se 'LANG' nem existir. Fica em PT.
    }
}

void msgs_set_language(const char *lang_code)
{
    if (!lang_code) return;
    if (strncmp(lang_code, "pt", 2) == 0) {
        idioma_atual = 0;
    } else if (strncmp(lang_code, "en", 2) == 0) {
        idioma_atual = 1;
    } else if (strncmp(lang_code, "es", 2) == 0) {
        idioma_atual = 2;
    } else if (strncmp(lang_code, "zh", 2) == 0) {
        idioma_atual = 3;
    } else {
        idioma_atual = 1;
    }
}
/**
 * Nome da função: get_msg
 * O que faz: Entrega a string de texto final pronta para ser exibida, já no idioma correto.
 * Como funciona: Acessa diretamente a memória da matriz 'mensagens'.
 * Acessamos [idioma_atual] para escolher a linha certa, e [id] para pegar a coluna (frase) certa.
 * Parâmetros:
 * - id: O identificador numérico da mensagem (vindo do 'enum Mensagem').
 * Retorno: Ponteiro de caractere constante (const char*) para a frase requisitada.
 * Onde é usada: Constantemente, quase sempre encapsulada sob a macro 'MSG()', em cada 'printf' ou 'fprintf'.
 * Observações: Este método garante um acesso O(1) ultra-rápido às frases, sem processamentos adicionais.
 */
const char* get_msg(enum Mensagem id)
{
    return mensagens[idioma_atual][id];
}