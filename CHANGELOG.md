# Changelog – NeonX

Todas as funcionalidades e melhorias do **NeonX** (versão C, multiplataforma) estão documentadas abaixo.  
O projeto é um colorizador de terminal com animações em tempo real, suporte a streaming, gradientes e verificação de integridade via assinatura digital.

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
