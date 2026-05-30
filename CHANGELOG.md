# Changelog – NeonX

Todas as funcionalidades e melhorias do **NeonX** (versão C, multiplataforma) estão documentadas abaixo.

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
- **Suporte a Paletas Customizadas:** O motor de renderização agora suporta offsets de fase RGB individuais por preset. Isso permite cores precisas e fiéis para temas como `Dracula` e `Synthwave`.
- **Novos Presets Estéticos:** Adição de 7 novos temas: `vaporwave`, `ocean`, `forest`, `blood`, `hacker`, `synthwave` e `dracula`.
- **Interface de Ajuda Expandida:** O menu `--help` agora lista dinamicamente todos os presets disponíveis em múltiplos idiomas.

### Corrigido
- **Blindagem do Parser de Argumentos:** Corrigida falha onde flags subsequentes (ex: `-L`) eram "engolidas" se passadas como valor para a flag `--preset`.
- **Validação de Presets:** O sistema agora valida se o nome do preset existe antes de iniciar a renderização, exibindo um erro amigável em caso de falha.
- **Ajuste Cromático do Tema Dracula:** O preset `dracula` foi reconfigurado para usar tons sombrios de roxo e rosa, eliminando cores quentes indesejadas.

## [2.2.0-STABLE] – 2026

### Adicionado
- **Sistema Centralizado de Estilos (`src/style.h`):** Introdução de um arquivo dedicado para gerenciar macros de formatação ANSI, incluindo o novo logotipo dinâmico `LOGO_NEONX` (com as cores N(vermelho) e(amarelo) o(verde) n(ciano) X(azul)) e padronização visual das mensagens de erro e alertas.
- **Expansão Massiva de Internacionalização (i18n):** O suporte a idiomas foi expandido de 13 para 19 línguas (incluindo Polonês, Indonésio, Turco, entre outros). A detecção automática e tratamento de fallbacks foram aprimorados.
- **Integração de Keygen Otimizada:** A ferramenta interna `tools/keygen.c` agora suporta extração direta do array C (`--print-hex`), formatando a chave pública em `8 bytes` por linha, facilitando a atualização imediata no cabeçalho `src/integrity.h`.
- **Refinamento do Build System:** A macro `GENERIC_NEONX_KEY` injetada dinamicamente pelo `build.sh` agora trata adequadamente a nova extração em múltiplas linhas do keygen.

## [2.1.9-STABLE] – 2026

### Corrigido
- **Segurança e Melhores Práticas (Format Strings):** Implementação de `__attribute__((format(printf, ...)))` em todas as funções wrapper de log/erro (`src/main.c`, `src/render.c`). Isso permite que o compilador valide os argumentos em tempo de compilação, prevenindo vulnerabilidades de *Format String Attack*.
- **Limpeza de Código:** Remoção de diretivas `#pragma GCC diagnostic` manuais, centralizando a gestão de avisos no sistema de build.
- **Estabilidade no Windows:** Adicionada a macro `_CRT_SECURE_NO_WARNINGS` globalmente para silenciar avisos de funções inseguras da biblioteca padrão da Microsoft.

## [2.1.8-INTERNAL] – 2026

### Corrigido
- **Linkedicao Windows (MSVC/Zig):** Ajuste na nomenclatura das bibliotecas estáticas de `.a` para `.lib` no ambiente Windows para satisfazer os requisitos do linker nativo.
- **Avisos de Compilação:** Adicionada a flag `-Wno-format-nonliteral` no script de build para suportar o sistema de internacionalização (i18n) que utiliza strings de formato dinâmicas.

## [2.1.7-INTERNAL] – 2026

### Corrigido
- **Compatibilidade macOS (Bash 3.2):** Refatoração do script `build.sh` para remover o uso de arrays associativos (`declare -A`), substituindo-os por lógica compatível com a versão legada do Bash presente no macOS.
- **Portabilidade de Build:** Correção na detecção do dispositivo nulo (`NUL` vs `/dev/null`) e tratamento de flags de endurecimento (hardening) específicas para cada plataforma.

## [2.1.6-STABLE] – 2026

### Corrigido
- **Overflow Crítico em `neonx_isqrt64`:** Corrigido o erro de truncamento na função de raiz quadrada que afetava cálculos de distância em coordenadas grandes, resolvendo bugs na flag de opacidade (`-o`) e em shaders radiais.

### Adicionado
- **Suíte de Testes Abrangente:** Implementação de `tests/unit/test_comprehensive.c` que valida precisão matemática, lógica de cores e efeitos visuais.
- **Modo de Teste Verboso:** Nova flag `./build.sh --test-debug` para depuração detalhada de erros de ponto fixo e visualização de resultados intermediários.

## [2.1.5-STABLE] – 2026

### Adicionado
- **Aleatoriedade de Fase Aprimorada:** Nova função `neonx_random_phase()` que utiliza entropia real do sistema (`/dev/urandom` ou `BCryptGenRandom`) para gerar a fase inicial das animações.
- **Alta Precisão Fracionária:** A fase inicial agora aproveita os 16 bits de precisão do sistema de ponto fixo, garantindo que cada execução (especialmente no modo estático `-S`) resulte em um gradiente visualmente único e imperceptivelmente variado.
- **Proteção contra Overflows:** A nova lógica de fase aplica máscaras de bits (30 bits) para evitar estouros aritméticos durante cálculos de shaders complexos, mantendo a estabilidade.

### Alterado
- **Unificação de Flags de Fase:** As flags `-p` e `-P` agora são sinônimas e tratadas de forma consistente para definir uma fase manual (seed).
- **Limpeza de Estruturas:** Removido o campo redundante `start_phase_fixed` da estrutura de opções, consolidando a lógica no motor de renderização.

## [2.1.4-STABLE] – 2026

### Corrigido
- **Bug de Opacidade (-o):** Correção de erro de escala na aritmética de ponto-fixo que causava tela preta ao utilizar a flag de opacidade em certos modos (especialmente `-m 7`).
- **Limpeza de Código:** Remoção de funções legadas e redundantes de configuração de opacidade.

## [2.1.3-STABLE] – 2026

### Adicionado
- **Segurança e Robustez Numérica:** Substituição do parser manual de ponto-fixo por `secure_str_to_fixed`, utilizando `strtod` e verificação de `errno` para evitar comportamentos indefinidos com entradas malformadas.
- **Gestão de Memória em Sinais:** O tratador de `SIGINT` (Ctrl+C) agora libera explicitamente o buffer de conteúdo principal (`free_content`) antes de encerrar o processo, garantindo encerramento limpo.

### Alterado
- Refatoração da validação de argumentos numéricos (`-o`, `-F`, `-m`, `-c`, `-P`) com mensagens de erro mais precisas e validação rigorosa de limites.
- Migração interna: a opacidade agora é configurada via `neonx_set_opacity` (ponto-fixo) em vez de strings, melhorando a consistência com o `neonx_core`.

## [2.1.2-STABLE] – 2026

### Adicionado
- **Expansão de Testes i18n:** Novos casos de teste para Espanhol (`es`) e Chinês (`zh`) no `integration_test.sh`.
- **CI/CD Robusto:** O workflow do GitHub Actions agora valida corretamente caracteres multi-byte e UTF-8 em diferentes sistemas operacionais (Linux, macOS, Windows).

### Alterado
- **Melhoria na Detecção de Idioma:** O sistema agora isola corretamente o código de idioma em sistemas Unix quando `LANG` contém sufixos de codificação (ex: de `pt_BR.UTF-8` para `pt`).
- **Ordem de Inicialização:** A chamada `setlocale(LC_ALL, "")` foi movida para o início de `init_system_context`, garantindo que todas as bibliotecas e mensagens sigam o locale do sistema desde o primeiro byte.
- **Refatoração do Sistema de Assinatura:** A ferramenta `sign_binary` agora envia a assinatura via STDOUT, permitindo que o `build.sh` gerencie a anexação ao binário de forma mais flexível.

## [2.1.1-STABLE] – 2026

### Adicionado
- **Arquitetura Modular (NeonX Core):** O motor de renderização e a matemática de ponto fixo foram divididos em `src/math_fixed.c`, `src/shader_effects.c` e `src/render_core.c`, permitindo o uso do NeonX como uma biblioteca modular e segura.
- **Suporte Robusto a WebAssembly (WASM):**
  - Implementação do bridge WASM em `src/main_wasm.c`.
  - Interface HTML interativa (`index.html`) para demonstração dos shaders no navegador.
  - Exportação de funções críticas via Emscripten para manipulação de cores em tempo real via JS.
- **Sistema de Assinatura Efêmera:** 
  - O `build.sh` agora gera chaves Ed25519 temporárias durante a compilação para assinar binários locais e de CI/CD automaticamente.
  - Adicionado suporte à macro `GENERIC_PUBLIC_KEY` para injeção de chaves públicas em tempo de compilação.
  - O comando `--version` agora diferencia entre `OFFICIAL_BY_INRRYOFF` e `VALID_SIG_BY_COMMUNITY` baseando-se na chave utilizada.
- **Melhorias no `build.sh`:**
  - Adicionado suporte nativo para compilação WASM via Emscripten (`./build.sh` -> Opção 2).
  - Otimização do pipeline de cross-compilação com Zig.
  - Novo sistema de gerenciamento de ferramentas internas (keygen/sign) durante o build.
- **Novas Flags de CLI:**
  - `-max-lines <num>`: Permite configurar o limite do buffer de entrada (evita estouro de memória em arquivos gigantes).
  - `-P <valor>`: Permite definir um deslocamento (offset) manual de fase para animações.

### Alterado
- Refatoração completa dos headers para evitar dependências circulares entre `shaders.h` e `neonx_core.h`.
- Correção de erro de compilação em `main.c` (falta de `<stdarg.h>`) que impedia o build em alguns compiladores rigorosos.
- Melhoria na precisão da interpolação linear da tabela de senos.

## [2.0.4-STABLE] – 2026

### Adicionado

#### Integridade e segurança (`integrity.c` / `integrity.h`)
- **Assinatura Ed25519 embutida** – o binário é assinado internamente (últimos 128 bytes em hexadecimal).  
- `check_integrity()` lê seu próprio executável, extrai a assinatura e valida com uma chave pública fixa (via Monocypher).  
- Comando `--verify-sig` retorna `OK` (0) ou `FAIL` (1) – útil para scripts de verificação.  
- Status da integridade é mostrado em `--version` (`OFFICIAL_BY_INRRYOFF` ou `MODIFIED`).

#### Motor de shaders em ponto‑fixo (`shaders.c` / `shaders.h`)
- **Aritmética Q16.16** – sem ponto flutuante, garantindo desempenho consistente.  
- **Tabela de seno** (`sin_lut_fixed`) com 4096 entradas, inicializada por `init_lut()`.  
- `fast_sin_fixed()` – interpolação linear a partir da LUT.  
- `fast_dist_fixed()` – distância euclidiana via raiz quadrada inteira (`isqrt64`).  
- **12 modos de animação** (`-m 0..11`):  
  - `0` – gradiente senoidal com ângulo ajustável (`-A`).  
  - `1` – pôr do sol (senos em X e Y).  
  - `2` – fase uniforme (cor única em movimento).  
  - `3` – onda vertical.  
  - `4` – efeito “retro” ondulado.  
  - `5` – anéis concêntricos a partir do centro.  
  - `6` – produto de senos (tecido ondulante).  
  - `7` – gradiente horizontal decrescente.  
  - `8` – gradiente vertical decrescente.  
  - `9` – varredura entrelaçada (efeito CRT).  
  - `10` – **Matrix** (pulsos + scanlines + centelhas aleatórias).  
  - `11` – pulso radial (distância modulada).  
- **Ângulo do gradiente** (`-A`) – rotação do padrão em graus (pré‑calculado em `grad_cos_fixed` / `grad_sin_fixed`).  
- **Opacidade das bordas** (`-o 0..1`) – atenuação das cores nas extremidades.  
- **Modo quantizado** (`--quantized`) – reduz cada canal para 5 bits (efeito visual retrô).

#### Interface com o terminal (`terminal.c` / `terminal.h`)
- Estrutura `Content` para armazenar até 1024 linhas (wchar_t).  
- `sleep_us()` – microssegundos multiplataforma (Windows: `Sleep`, Unix: `usleep`).  
- `free_content()` – libera memória das linhas.  
- Exibição de versão, licença e ajuda com suporte a português/inglês.  
- `handle_sigint()` – restaura o cursor e o buffer do terminal ao receber SIGINT (Ctrl+C).  
- Controle de FPS (`-F`) – converte o valor para microssegundos de espera entre frames.

#### Sistema de mensagens (`msgs.c` / `msgs.h`)
- Internacionalização completa: português (índice 0) e inglês (índice 1).  
- Seleção automática via `LANG` (prefixo `pt`).  
- Macro `MSG(id)` para acesso fácil às strings.  
- Mensagens de erro, ajuda, licença e versão organizadas em enum.

#### Criptografia e utilitários (`monocypher.c` / `monocypher.h`)
- **Monocypher** – implementação leve e segura (domínio público / BSD‑2‑Clause).  
- Módulos usados no NeonX:  
  - `crypto_eddsa_check` – validação de assinatura.  
  - `crypto_eddsa_trim_scalar` – preparação de escalares.  
  - `crypto_eddsa_reduce`, `crypto_eddsa_mul_add` – suporte à aritmética de curva.  
- (Outros algoritmos da biblioteca – Chacha20, Poly1305, BLAKE2b, Argon2, X25519 – estão presentes mas não utilizados diretamente pelo aplicativo principal.)

#### Processamento principal (`main.c`)
- **Leitura da entrada** – suporte a pipe e redirecionamento.  
- **Dois modos de operação**:  
  - **Modo normal** – armazena todas as linhas e anima repetidamente (com duração limitada por `-d`).  
  - **Modo streaming** (`-L`) – processa linha a linha, sem buffer, ideal para `tail -f`.  
- **Parsing de argumentos** – suporte a opções curtas (`-h`, `-v`, `-m`, `-s`, `-f`, `-d`, `-A`, `-p`, `-S`, `-c`, `-o`, `-F`, `-L`) e longas (`--lang`, `--verify-sig`, `--preset`, `--quantized`, `--spin`, `--help`, `--version`, `--license`).  
- **Conversão de valores reais** (`str_to_fixed`) – aceita decimais como `0.2` e converte para ponto‑fixo.  
- **Presets** (`--preset`): `cyberpunk`, `retro`, `matrix`, `sunset` – ajustam modo, velocidade, frequência e ângulo do gradiente.  
- **Modo `--spin`** – gera 60 códigos ANSI `38;2;R;G;B` representando um ciclo de cores completo (útil para scripts externos).  
- **Modo estático** (`-S`) – exibe apenas um frame (sem animação).  
- **Loop de animação** – recalcula cores a cada frame usando `get_color_fast()`, atualiza a fase (`phase += speed`), e reescreve a tela utilizando sequências de escape `\033[%dA` (move o cursor para cima).  
- **Tratamento de caracteres de escape** – qualquer `0x1B` na entrada é substituído por `'?'` para não interferir na formatação ANSI.

### Build e distribuição (`build.sh`)

- **Builder** – script bash com suporte a compilação nativa (Clang) e cruzada (Zig).  
- **Flags de compilação** (corrigido em relação a versões anteriores do changelog):  
  ```bash
  PERF_FLAGS="-O3 -ffast-math -flto -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"
  TUNE_FLAGS="-march=armv8.2-a -mtune=cortex-a75"  # apenas para ARM64
  ```

- **Geração de assinaturas** (interna via ferramenta `sign_binary` + externa via `minisign`).  
- **Pacotes ZIP** – cada binário é empacotado junto com sua assinatura Minisign.  
- **Atualização de hashes SHA256** – armazenados em `KEYS_DIR/SHA256SUMS.txt`.  
- **Plataformas suportadas pelo builder**:  
  - Nativa para Cortex-A75 (para mudar use a flag `TUNE_FLAGS="-march=native"` ou `TUNE_FLAGS="-march=sua actetura -mtune=seu processador"`
  - Linux x64, x86, ARM64, ARM32 (musl)  
  - Windows x64, x86 (mingw)  
  - macOS arm64, x64  

### Correções e melhorias internas

- **Portabilidade Windows** – `SetConsoleOutputCP(CP_UTF8)` e ativação de sequências virtuais (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`).  
- **Substituição de `printf` por `write`** no loop principal – reduz buffering e aumenta performance.  
- **Limpeza de memória** – todas as alocações são liberadas ao final ou no `SIGINT`.  
- **Cálculo da distância máxima** (`max_dist_fixed`) é feito uma única vez por frame, otimizando a opacidade das bordas.  

## Como usar (exemplos)

```bash
# Uso básico – animação matrix durante 5 segundos
cat texto.txt | neonx -m 10 -s 0.5 -d 5

# Streaming de log com efeito sunset a 60 FPS
tail -f /var/log/syslog | neonx -L -m 1 -F 60

# Verificar assinatura do binário
neonx --verify-sig
# Deve retornar ok para binarios oficiais e fail para binarios compilados localmente

# Exibir versão e status de integridade
neonx --version
```

## Notas de desenvolvimento

- **Dependências externas** – Nenhuma. Monocypher é fornecido junto ao código.
- **Testado em** – Linux (x86_64, ARM64), Windows (MSYS2/MinGW), macOS (Intel/Apple Silicon), Android (Termux).  
- **Licença do código** – O binário exibe uma licença de uso com `--license` (atribuição obrigatória, proibida venda, distribuição gratuita permitida).  
- **Licença do Monocypher** – BSD‑2‑Clause ou CC0 (domínio público).
