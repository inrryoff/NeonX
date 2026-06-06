# Changelog – NeonX

Todas as funcionalidades e melhorias do **NeonX** (versão C, multiplataforma) estão documentadas abaixo.

# [2.2.7-STABLE] – 2026

### Adicionado
- **4 novos modos de shader (12–15):** O motor de renderização agora conta com 16 modos no total.
  - `12` — **Tunnel:** zoom radial com ilusão de profundidade e rotação ao redor do centro.
  - `13` — **Diamante:** distância de Manhattan (`|x-cx| + |y-cy|`) gerando padrão de losango.
  - `14` — **Turbulência:** três harmônicos senoidais sobrepostos em frequências distintas, efeito tipo Perlin inteiro.
  - `15` — **Scan Line:** faixa de luz que varre verticalmente com fundo em meio-tom.
- **10 novos presets temáticos:** `wormhole`, `vortex`, `diamond`, `prism`, `chaos`, `acid`, `nebula`, `radar`, `scanner`, `crt`. Total sobe para 31 presets.
- **Gradiente fixo por preset (`use_gradient`):** A struct `PresetConfig` agora suporta duas cores RGB exatas (`gr/gg/gb` e `gr2/gg2/gb2`) para interpolação direta sem paleta senoidal. Presets `fire` e `lava` usam esse mecanismo — eliminando qualquer possibilidade de cor fora do escopo definido.
- **Ferramenta `calc` (`tools/calc.c`):** Calculadora de ponto fixo Q16.16 para desenvolvimento de presets.
  - `./calc <valor>` — converte decimal para Q16.16.
  - `./calc <graus> --graus` — converte ângulo para campo `angle` do preset.
  - `./calc palets [nome|família]` — lista offsets senoidais por família de cor com HEX aproximado.
  - `./calc hex "#RRGGBB"` — converte HEX direto para campos `use_gradient` e sugere paleta senoidal mais próxima.
  - `./calc gen <família> [n]` — gera N variações calculadas via HSL para a família de cor, com HEX e offsets prontos.
- **Detecção automática de modo de cor:** `detect_color_mode()` em `terminal.c` lê `$COLORTERM` e `$TERM` e seleciona automaticamente entre true color (`\033[38;2;R;G;Bm`) e 256 cores (`\033[38;5;Nm`). Terminais sem suporte a true color usam o cubo 6×6×6 como fallback.

### Corrigido
- **`-A` funcionando em todos os modos:** O offset do ângulo (`grad_cos_fixed` / `grad_sin_fixed`) era aplicado apenas no `default` (modo 0). Agora é aplicado após o `switch`, afetando todos os 16 modos. Presets com modos de direção própria (3, 5, 7, 8, 11, 13) foram corrigidos para `angle = -65536` (desabilitado).
- **Gradiente do vaporwave:** O preset `vaporwave` usava ângulo `4915200` (75°) que distorcia o gradiente vertical do modo 3. Corrigido para `angle = -65536`.
- **Velocidade e cores do `fire`:** Speed reduzido de `26214` para `16384`. Cores corrigidas via `use_gradient` com vermelho `#DC1E00` → laranja `#FF9400` e ângulo de 120°, eliminando verde e roxo indesejados.
- **Cores do `lava`:** Corrigido via `use_gradient` com vermelho escuro `#780000` → laranja escuro `#DC3C00`, sem ciclar pelo espectro completo.

### Otimizado
- **`CliDriverCtx` persistente entre linhas no `run_buffered_mode`:** O contexto de cor (`last_r/g/b`) agora é mantido entre todas as linhas do mesmo frame, evitando reemissão desnecessária de escapes de cor no primeiro pixel de cada linha. Reduz o volume de dados enviados ao PTY em frames com cores estáveis.
- **`memcpy` direto para strings fixas no loop de render:** Substituição de `safe_append` (que chama `vsnprintf`) por `memcpy` direto para os bytes fixos `\r` e `\033[0m\033[K\n`, reduzindo overhead por linha.
- **Remoção de FPU do código de produção:** Todas as ocorrências de `float`/`double` foram eliminadas dos arquivos de produção.
  - `math_fixed.c`: `sin()` da libm substituído por série de Taylor em ponto fixo puro (CORDIC) para popular a LUT — sem dependência de `<math.h>` em runtime.
  - `render_core.c`: `precalc_gradient_angle` substituído por `neonx_fast_sin_fixed` + `FIXED_PI_2`.
  - `render.c`: todos os `FLOAT_TO_FIXED(float)` substituídos por shifts diretos.
  - `main.c`: `strtod` substituído por `secure_str_to_fixed` (parser inteiro puro); `0.2f` → constante `13107`.
  - `math_fixed.h`: macros `FLOAT_TO_FIXED` e `FIXED_TO_FLOAT` removidas.
- **Adição de `DEG_TO_RAD_FIXED` em `math_fixed.h`:** Constante inteira `1143` (π/180 × 65536) para conversão de graus sem FPU.

# [2.2.6-STABLE] – 2026

### Adicionado
- **Adição de Presets:** O arquivo `shaders.c` foi atualizado e para incluir novos Presets.
- **Adição de Ferramentas:** A pasta `tools/` agora tem um arquivo `gen_config.c` que substitui o script em python `sync_build.py`, que cuida da geração de IDs de build.

## [2.2.5-STABLE] – 2026

### Adicionado
- **Assinatura Efêmera Automática:** O sistema de build agora gera chaves criptográficas temporárias automaticamente caso não existam, garantindo que o binário esteja sempre assinado para verificações de integridade sem configuração manual.
- **Isolamento Arquitetural Modular:** O arquivo `build_config.h` foi movido definitivamente para o diretório `headers/`, limpando o escopo da raiz e eliminando riscos de conflito em diretivas de inclusão `-I`.

## [2.2.4-STABLE] – 2026

### Adicionado
- **Sincronização de Fase do Motor:** Implementação de um sistema de sincronização técnica baseado em hashes de metadados para garantir fidelidade cromática absoluta em todas as plataformas.
- **Build ID Dinâmico:** Introdução de identificadores de build hexadecimais (`DX22`, `FX11`) para rastreamento de variantes de compilação e controle de qualidade.
- **Proteção TTY Inteligente:** Implementação de detecção automática de terminal (`isatty`) para prevenir execuções sem dados e limpar sequências ANSI em redirecionamentos (ex: `neonx --license > LICENSE.txt`).

### Otimizado
- **Filtro de Fidelidade Cromática:** Refinamento da lógica de cores para garantir estabilidade visual e prevenir glitches em builds não sincronizados.

## [2.2.3-STABLE] – 2026

### Adicionado
- **API Pública Unificada (`headers/neonx.h`):** Criação de um cabeçalho consolidado para facilitar o uso do NeonX Core como biblioteca em projetos externos, documentando as principais interfaces de renderização e matemática.
- **Documentação de Arquitetura:** Inclusão do arquivo `ARCHITECTURE.md` detalhando os princípios de design, fluxo de dados e estrutura do projeto.
- **Robustez na Inicialização:** Refatoração do carregamento da chave de integridade em `core/integrity.c` para mitigar riscos de race conditions e melhorar a segurança do estado global.

### Otimizado
- **Cache de Comprimento de Linhas:** Implementação de cache para comprimentos de strings na estrutura `Content`, eliminando chamadas redundantes a `wcslen` e `wcsnlen` durante o loop de animação, resultando em ganho significativo de performance.
- **Simplificação de Dependências:** Refatoração interna para utilizar o novo cabeçalho `neonx.h`, reduzindo o acoplamento e simplificando a manutenção.

### Alterado
- **Documentação Técnica:** Adicionados comentários sobre o tratamento de caracteres fora do BMP (Emojis) em sistemas Windows devido à natureza UTF-16 do `wchar_t`.

## [2.2.2-STABLE] – 2026

### Adicionado
- **Gradientes Customizados (v2.2.2):** Introdução das flags `--color1` e `--color2` (ou `--c1` / `--c2`), permitindo transições suaves entre cores hexadecimais arbitrárias (ex: `#FF0000` para `#FFA500`).
- **Paridade Total WASM:** A interface WebAssembly agora exporta as mesmas funcionalidades do CLI, incluindo suporte a presets (`neonx_wasm_set_preset`), controle de paleta (`neonx_wasm_set_palette_offsets`) e reset de estado.
- **Refatoração RenderDriver:** Unificação da arquitetura de saída entre CLI e WASM utilizando a abstração `RenderDriver`, garantindo que toda lógica de cor e caractere passe por uma interface agnóstica de plataforma.

### Alterado
- **Internacionalização:** Atualização das mensagens de ajuda em todos os 19 idiomas para incluir as novas opções de gradiente personalizado.
- **Versão Global:** Salto de versão para 2.2.2 em todos os componentes do ecossistema (CLI, Core e WASM).

## [2.2.1-STABLE] – 2026

### Adicionado
- **Suporte a Paletas Customizadas:** O motor de renderização agora supports offsets de fase RGB individuais por preset. Isso permite cores precisas e fiéis para temas como `Dracula` e `Synthwave`.
- **Novos Presets Estéticos:** Adição de 7 novos temas: `vaporwave`, `ocean`, `forest`, `blood`, `hacker`, `synthwave` e `dracula`.
- **Interface de Ajuda Expandida:** O menu `--help` agora lista dinamicamente todos os presets disponíveis em múltiplos idiomas.

### Corrigido
- **Blindagem do Parser de Argumentos:** Corrigida falha onde flags subsequentes (ex: `-L`) eram "engolidas" se passadas como valor para a flag `--preset`.
- **Validação de Presets:** O sistema agora valida se o nome do preset existe antes de iniciar a renderização, exibindo um erro amigável em caso de falha.
- **Ajuste Cromático do Tema Dracula:** O preset `dracula` foi reconfigurado para usar tons sombrios de roxo e rosa, eliminando cores quentes indesejadas.

## [2.2.0-STABLE] – 2026

### Adicionado
- **Sistema Centralizado de Estilos (`headers/style.h`):** Introdução de um arquivo dedicado para gerenciar macros de formatação ANSI, incluindo o novo logotipo dinâmico `LOGO_NEONX` e padronização visual das mensagens de erro e alertas.
- **Expansão Massiva de Internacionalização (i18n):** O suporte a idiomas foi expandido de 13 para 19 línguas (incluindo Polonês, Indonésio, Turco, entre outros).
- **Integração de Keygen Otimizada:** A ferramenta interna `tools/keygen.c` agora suporta extração direta do array C (`--print-hex`).
- **Refinamento do Build System:** A macro `GENERIC_NEONX_KEY` injetada dinamicamente pelo `build.sh` agora trata adequadamente a nova extração em múltiplas linhas do keygen.

## [2.1.9-STABLE] – 2026

### Corrigido
- **Segurança e Melhores Práticas (Format Strings):** Implementação de `__attribute__((format(printf, ...)))` em todas as funções wrapper de log/erro.
- **Limpeza de Código:** Remoção de diretivas `#pragma GCC diagnostic` manuais.
- **Estabilidade no Windows:** Adicionada a macro `_CRT_SECURE_NO_WARNINGS` globalmente.

## [2.1.8-INTERNAL] – 2026

### Corrigido
- **Linkedicao Windows (MSVC/Zig):** Ajuste na nomenclatura das bibliotecas estáticas de `.a` para `.lib`.
- **Avisos de Compilação:** Adicionada a flag `-Wno-format-nonliteral` no script de build.

## [2.1.7-INTERNAL] – 2026

### Corrigido
- **Compatibilidade macOS (Bash 3.2):** Refatoração do script `build.sh` para remover o uso de arrays associativos (`declare -A`).
- **Portabilidade de Build:** Correção na detecção do dispositivo nulo e tratamento de flags de endurecimento por plataforma.

## [2.1.6-STABLE] – 2026

### Corrigido
- **Overflow Crítico em `neonx_isqrt64`:** Corrigido o erro de truncamento na função de raiz quadrada que afetava cálculos de distância em coordenadas grandes.

### Adicionado
- **Suíte de Testes Abrangente:** Implementação de `tests/unit/test_comprehensive.c`.
- **Modo de Teste Verboso:** Nova flag `./build.sh --test-debug`.

## [2.1.5-STABLE] – 2026

### Adicionado
- **Aleatoriedade de Fase Aprimorada:** Nova função `neonx_random_phase()` com entropia real do sistema.
- **Alta Precisão Fracionária:** A fase inicial aproveita os 16 bits de precisão do sistema de ponto fixo.
- **Proteção contra Overflows:** Máscaras de bits (30 bits) para evitar estouros aritméticos.

### Alterado
- **Unificação de Flags de Fase:** As flags `-p` e `-P` agora são sinônimas.
- **Limpeza de Estruturas:** Removido o campo redundante `start_phase_fixed`.

## [2.1.4-STABLE] – 2026

### Corrigido
- **Bug de Opacidade (-o):** Correção de erro de escala na aritmética de ponto-fixo.

## [2.1.3-STABLE] – 2026

### Adicionado
- **Segurança e Robustez Numérica:** Substituição do parser manual por `secure_str_to_fixed`.
- **Gestão de Memória em Sinais:** O tratador de `SIGINT` agora libera explicitamente o buffer de conteúdo.

## [2.1.2-STABLE] – 2026

### Adicionado
- **Expansão de Testes i18n:** Novos casos de teste para Espanhol e Chinês.
- **CI/CD Robusto:** Validação de caracteres multi-byte em Linux, macOS e Windows.

### Alterado
- **Melhoria na Detecção de Idioma:** Isolamento correto do código de idioma com sufixos de codificação.
- **Ordem de Inicialização:** `setlocale(LC_ALL, "")` movida para o início de `init_system_context`.
- **Refatoração do Sistema de Assinatura:** `sign_binary` agora envia a assinatura via STDOUT.

## [2.1.1-STABLE] – 2026

### Adicionado
- **Arquitetura Modular (NeonX Core):** Motor dividido em `core/math_fixed.c`, `core/shader_effects.c` e `core/render_core.c`.
- **Suporte Robusto a WebAssembly (WASM):** Bridge WASM em `main_wasm.c` e interface HTML interativa.
- **Sistema de Assinatura Efêmera:** Chaves Ed25519 temporárias geradas automaticamente no build.
- **Novas Flags de CLI:** `-max-lines` e `-P`.

## [2.0.4-STABLE] – 2026

### Adicionado
- **Assinatura Ed25519 embutida** com verificação via Monocypher.
- **Motor de shaders em ponto-fixo** Q16.16 com 12 modos de animação.
- **Interface com o terminal** via `Content`, `sleep_us`, `free_content`, `handle_sigint`.
- **Sistema de mensagens i18n** com detecção automática via `LANG`.
- **Monocypher** integrado para funções criptográficas.
