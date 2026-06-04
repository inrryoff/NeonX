<!-- ────────────────────────────────────────────────
     NeonX — Colorizador de Terminal Profissional
     README v2.2.5-STABLE
     ──────────────────────────────────────────────── -->

<div align="center">

<h1>
  <img src="https://raw.githubusercontent.com/inrryoff/NeonX/main/assets/neonx-icon.png" width="32" align="top" alt="">
  NeonX
</h1>

**Um Colorizador de Terminal Profissional – rápido, bonito e multi‑plataforma**

[![Versão](https://img.shields.io/badge/version-2.2.6--STABLE-blue?style=flat-square)](https://github.com/inrryoff/NeonX/releases)
[![NeonX Web](https://img.shields.io/badge/NeonX-Web_Demo-0a0a0f?style=flat-square&logo=terminal&logoColor=00ffcc)](https://neonx-web.netlify.app/)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Android%20%7C%20Windows%20%7C%20WASM-brightgreen?style=flat-square)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red?style=flat-square)](./LICENSE)
[![Testes](https://img.shields.io/github/actions/workflow/status/inrryoff/NeonX/tests.yml?branch=main&label=tests&style=flat-square)](https://github.com/inrryoff/NeonX/actions/workflows/tests.yml)

<br>

![Escrito em C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![WebAssembly](https://img.shields.io/badge/WebAssembly-654FF0?style=for-the-badge&logo=webassembly&logoColor=white)
![Desenvolvido no Termux](https://img.shields.io/badge/Developed_on-Termux-blue?style=for-the-badge&logo=termux&logoColor=white)

</div>

---

## ✨ O que é o NeonX?

O **NeonX** transforma textos comuns em animações coloridas diretamente no terminal ou no navegador.  
Escrito do zero em **C puro**, ele usa matemática de ponto fixo e shaders procedurais para garantir **desempenho máximo** e **consumo mínimo de recursos**.

### 🎯 Por que usar o NeonX?

- **Alta performance** – loops otimizados, cache inteligente e zero alocações desnecessárias.
- **Multi‑plataforma** – funciona em Linux, macOS, Android (via Termux), Windows e até no navegador (WASM).
- **Sem dependências externas** – basta um terminal com suporte a 24-bit (true color) ou 8-bit.
- **Personalização total** – gradientes, ângulos, velocidade, modo stream para logs, presets temáticos e muito mais.
- **Internacionalização completa** – disponível em 19 idiomas, com detecção automática.

---

## 📦 Instalando e Compilando você mesmo

```bash
git clone https://github.com/inrryoff/NeonX.git
cd NeonX
make
sudo make install
```

### Ou utilizei o build.sh (recomendado)

```bash
# Clone o repositório
git clone https://github.com/inrryoff/NeonX.git
cd NeonX

# Compile usando o builder Shell
./build.sh
# E siga instruções no menu.
# Se quiser fazer tests use --test
```

---

## 📦 Instalação já compilada (oficial)

Escolha sua plataforma e siga as instruções para instalar a versão **v2.2.6-STABLE**:

### 🐧 Linux
<details>
<summary>Visualizar instruções</summary>

```bash
# 1. Baixe o pacote x64
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.2.6/neonx_linux-x64.zip

# 2. Extraia e instale
unzip neonx_linux-x64.zip
sudo mv neonx /usr/local/bin/
sudo chmod +x /usr/local/bin/neonx
```
</details>

### 🤖 Android (Termux)
<details>
<summary>Visualizar instruções</summary>

```bash
# 1. Baixe o pacote ARM64
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.2.6/neonx_linux-arm64.zip

# 2. Extraia e instale no PATH do Termux
unzip neonx_linux-arm64.zip
mv neonx $PREFIX/bin/
chmod +x $PREFIX/bin/neonx
```
</details>

### 🍏 macOS
<details>
<summary>Visualizar instruções</summary>

```bash
# 1. Baixe o pacote (arm64 para Apple Silicon)
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.2.6/neonx_macos-arm64.zip

# 2. Extraia e remova a quarentena
unzip neonx_macos-arm64.zip
sudo mv neonx /usr/local/bin/
sudo chmod +x /usr/local/bin/neonx
sudo xattr -d com.apple.quarantine /usr/local/bin/neonx
```
</details>

### 🪟 Windows (CMD & PowerShell)
<details>
<summary>Visualizar via CMD</summary>

```cmd
:: 1. Baixe usando curl nativo do Windows
curl.exe -LO https://github.com/inrryoff/NeonX/releases/download/v2.2.6/neonx_windows-x64.zip

:: 2. Extraia e mova para o System32 (necessário Admin)
tar -xf neonx_windows-x64.zip
move neonx.exe C:\Windows\System32\
```
</details>

<details>
<summary>Visualizar via PowerShell</summary>

```powershell
# 1. Baixe o pacote
curl.exe -LO https://github.com/inrryoff/NeonX/releases/download/v2.2.6/neonx_windows-x64.zip

# 2. Extraia e instale globalmente
Expand-Archive neonx_windows-x64.zip -DestinationPath . -Force
Move-Item -Path ".\neonx.exe" -Destination "C:\Windows\System32\" -Force
```
</details>

### 🌐 WebAssembly (WASM)
<details>
<summary>Visualizar instruções</summary>

O NeonX agora pode ser rodado diretamente no navegador como uma biblioteca.
```bash
# 1. Baixe o pacote WASM
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.2.6/neonx_wasm.zip

# 2. Extraia e rode um servidor local para testar
unzip neonx_wasm.zip
python3 -m http.server 8080
# Acesse http://localhost:8080 no seu navegador
```
</details>


## 🆕 Novidades da v2.2.5‑STABLE

- **Presets:** Novos Presets adicionas a lista você pode velogo a baixo na seção de **Presets**

---

## 📖 Guia de Uso

### Exemplos básicos

```bash
# Animação padrão
cat banner.txt | neonx

# Cores personalizadas
cat banner.txt | neonx -d 5 --color1 "#FF0000" --color2 "#FFA500"

# Logs em tempo real com preset
tail -f access.log | neonx --preset dracula -L

# Frame estático
echo "NeonX Engine" | neonx --preset synthwave -S
```

---

## 🛠️ Opções de linha de comando

| Opção | Descrição | Padrão |
|-------|-----------|--------|
| `-m [0-11]` | Modo de animação (0–11) | `0` |
| `-s [valor]` | Velocidade da transição | `0.2` |
| `-f [valor]` | Frequência da onda | `0.3` |
| `-d [seg]` | Duração total (`0` = infinito) | `0` |
| `-max-lines [val]` | Limite máximo de linhas | `10000` |
| `-A [graus]` | Ângulo do gradiente (0–360) | `0` |
| `-p, -P [valor]` | Seed fixa (determinística) | — |
| `-S` | Modo estático (apenas o primeiro frame) | desligado |
| `-c [largura]` | Força largura estática do gradiente | — |
| `-o [0-1]` | Opacidade horizontal / suavidade | `0.0` |
| `-O [0-1]` | Opacidade vertical (fading topo/base) | `0.0` |
| `-F [fps]` | Taxa de quadros por segundo | `20` |
| `-L` | Modo stream (linha a linha, ideal para `tail -f`) | desligado |
| `--fo [0-1]` | Modo fosco (reduz vivacidade) | `0` |
| `--preset [nome]` | Aplica um preset temático (ver tabela abaixo) | — |
| `--color1 [hex]` | Cor inicial do gradiente (ex: `#FF0000`) | — |
| `--color2 [hex]` | Cor final do gradiente (ex: `#FFA500`) | — |
| `--c1 [hex]`, `--c2 [hex]` | Atalhos para `--color1` e `--color2` | — |
| `--quantized` | Quantização de cores (maior performance) | desligado |
| `--no-ansi` | Desativa cores ANSI na saída | desligado |
| `--spin` | Gera apenas códigos ANSI puros (para scripts) | desligado |
| `--lang [código]` | Força o idioma (`pt`, `en`, `ja`, etc.) | automático |
| `--license` | Exibe os termos de licenciamento | — |
| `-v, --version` | Mostra versão e status do binário | — |
| `-h, --help` | Exibe este painel de ajuda interativo | — |

> **Observação:** Existe uma flag oculta (não documentada) que força o binário a reler a si mesmo e verificar sua integridade. O comando retornará o status no idioma atual: `OK` se íntegro, `FAIL` se modificado, ou uma mensagem de erro caso não seja possível ler o próprio executável.
---

## 🎨 Presets

Os presets definem um **modo de animação**, **paleta de cores** e **ângulo** específicos.  
A tabela abaixo descreve exatamente o que cada um faz, com base nos parâmetros internos.

| Preset | Modo | Paleta (comportamento das cores) | Ângulo |
|---|---|---|---|
| cyberpunk | 0 (horizontal) | Arco-íris tradicional | 45° |
| retro | 4 (ondas) | Paleta quente (vermelho / laranja) | 0° |
| matrix | 10 (shader matrix) | Verde com scanlines e brilho | 90° |
| sunset | 1 (sunset) | Tons quentes de pôr-do-sol | 30° |
| vaporwave | 3 | Rosa / roxo suave | 75° |
| ocean | 6 | Azul / verde oceânico | 50° |
| forest | 2 | Verde floresta | 25° |
| blood | 8 | Vermelho intenso | 0° |
| hacker | 0 (horizontal) | Verde terminal clássico | 0° |
| synthwave | 3 | Roxo / rosa neon | 90° |
| dracula | 1 (sunset) | Roxo escuro / tons sombrios | 45° |
| aurora | 5 | Verde e azul claro (estilo aurora boreal) | 0° |
| neon_tokyo | 4 (ondas) | Rosa e roxo vibrante | 0° |
| lava | 8 | Laranja e vermelho incandescente | 0° |
| ice | 11 | Azul e ciano gelado | 0° |
| fire | 3 | Cores quentes de chamas (vermelho/amarelo) | 0° |
| galaxy | 5 | Roxo e azul profundo espacial | 0° |
| toxic | 10 (shader matrix) | Verde radioativo / venenoso | 0° |
| midnight | 0 (horizontal) | Azul noturno escuro | 40° |
| rose | 1 (sunset) | Tons de rosa e avermelhado | 0° |
| vapor2 | 6 | Tons pastéis vibrantes de azul e rosa | 0° |

---

## 🌐 Idiomas suportados

Use o **código de dois caracteres** com `--lang`.  
**Exemplo:** `neonx --lang pt` (português), `neonx --lang ja` (japonês).

| Código | Idioma | Código | Idioma |
|:---|:---|:---|:---|
| `pt` | Português | `en` | Inglês |
| `es` | Espanhol | `fr` | Francês |
| `de` | Alemão | `it` | Italiano |
| `ru` | Russo | `zh` | Chinês |
| `ja` | Japonês | `ko` | Coreano |
| `tr` | Turco | `pl` | Polonês |
| `id` | Indonésio | `ar` | Árabe |
| `bg` | Búlgaro | `el` | Grego |
| `hi` | Hindi | `th` | Tailandês |
| `km` | Khmer | | |

> O idioma é detectado automaticamente; `--lang` força um específico.

---

## 🏗️ NeonX como biblioteca

```c
#include "neonx.h"

RenderDriver driver = {
    .set_color   = my_set_color,
    .reset_color = my_reset_color,
    .put_char    = my_put_char,
    .ctx         = my_context
};

neonx_render_line(L"Hello", 5, y_fixed, phase, mode, cx, cy, max_dist, &driver);
```

Documentação completa em [ARCHITECTURE.md](./ARCHITECTURE.md).

---

## 💖 Apoie o projeto

- ⭐ **Dê uma estrela** no [GitHub](https://github.com/inrryoff/NeonX)
- ☕ **Doação**: [Clique aqui](https://tipa.ai/TIPARYITV51W4R3SLEG22KVVK9I1)
- 🧑‍💻 **Autor**: [@inrryoff](https://github.com/inrryoff)

---

## 📄 Licença

Licença customizada. Veja [LICENSE](./LICENSE).

---

<div align="center">
  <sub>Feito com ☕ e C, no ritmo do terminal.</sub>
  <br>
  <sub>© 2024-presente, inrryoff</sub>
</div>