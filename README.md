# 🌈 NeonX – Ultra-High Performance Terminal Colorizer

[![Versão](https://img.shields.io/badge/version-2.1.5--STABLE-blue)](https://github.com/inrryoff/NeonX/releases)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Android%20%7C%20Windows%20%7C%20WASM-brightgreen)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red)](./LICENSE)
[![Testes](https://github.com/inrryoff/NeonX/actions/workflows/tests.yml/badge.svg)](https://github.com/inrryoff/NeonX/actions/workflows/tests.yml)

![C](https://img.shields.io/badge/C_Puro-00599C?style=for-the-badge&logo=c&logoColor=white)
![WASM](https://img.shields.io/badge/WebAssembly-654FF0?style=for-the-badge&logo=webassembly&logoColor=white)

**NeonX** é um colorizador de terminal de altíssima performance escrito em C puro. Utilizando matemática de ponto fixo e shaders procedurais, ele transforma textos estáticos em animações fluidas diretamente no seu terminal ou navegador.

---

## 🚀 Início Rápido

```bash
# 1. Pipe simples (Animação padrão)
cat banner.txt | neonx

# 2. Logs em tempo real (Modo Stream)
tail -f access.log | neonx -L --preset sunset

# 3. Animação com tempo limitado (5 segundos)
echo "NeonX Engine" | neonx --preset cyberpunk -d 5
```

---

## ✨ Destaques da v2.1.5-STABLE

- **Aleatoriedade de Fase Robusta:** Novo sistema de entropia para gradientes dinâmicos únicos a cada execução.
- **Performance Extrema:** Matemática de ponto fixo (Fixed-Point) para garantir zero overhead de FPU.
- **Segurança Reforçada:** Novo parser numérico robusto e blindagem Ed25519 (Monocypher).
- **Arquitetura Modular (NeonX Core):** Motor de renderização e matemática de ponto fixo agora divididos em módulos coesos e seguros.
- **Abstração de Driver:** Renderização agnóstica de plataforma (CLI e WASM compartilham o mesmo motor).

Para detalhes sobre a evolução técnica, leia o [CHANGELOG.md](./CHANGELOG.md). Para arquitetura profunda, veja o [DEVELOPMENT.md](./DEVELOPMENT.md).

---

## 🛠️ Configuração e Flags

<details>
<summary><b>Visualizar Tabela de Opções (CLI)</b></summary>

| Opção | Descrição | Padrão |
|---|---|---|
| `-m [0-11]` | Modo de animação (veja abaixo) | 0 |
| `-s [valor]` | Velocidade da animação | 0.2 |
| `-f [valor]` | Frequência das ondas | 0.3 |
| `-d [seg]` | Duração total (0 = infinito) | 0 |
| `-A [graus]` | Ângulo do gradiente (0-360) | 0 |
| `-o [0-1]` | Opacidade das bordas (Vignette) | 0.0 |
| `-F [fps]` | Taxa de quadros por segundo | 20 |
| `-L` | **Modo Stream:** Coloriza linha a linha (ideal para logs) | OFF |
| `-S` | **Modo Estático:** Renderiza apenas o primeiro frame | OFF |
| `-P [valor]` | Define a fase inicial (offset manual) | random |
| `-c [largura]`| Força uma largura fixa para o gradiente | auto |
| `-max-lines` | Limite máximo de linhas no buffer | 10000 |
| `--preset` | Presets: `cyberpunk`, `retro`, `matrix`, `sunset` | - |
| `--quantized`| Ativa modo 15-bit (retrô/blocos) | OFF |
| `--verify-sig`| Valida a integridade matemática do binário | - |
| `--lang [idioma]`| Seleciona o idioma: `pt`, `en`, `es`, `zh`, `ja`, etc. | auto |

</details>

---

## 🌍 Idiomas e Internacionalização

O NeonX detecta automaticamente o idioma do seu sistema:
- **Windows:** Utiliza a linguagem configurada na UI do sistema.
- **Linux/macOS/Android:** Utiliza a variável de ambiente `$LANG`.

Se desejar forçar um idioma específico, utilize a flag `--lang`:
```bash
# Forçar Chinês
neonx --lang zh --help

# Forçar Inglês (mesmo em sistema PT-BR)
neonx --lang en -v
```

---

---

## 🎨 Modos de Animação (Shaders)

<details>
<summary><b>Visualizar Lista de Shaders</b></summary>

| ID | Nome | Descrição |
|---|---|---|
| **0** | Linear | Gradiente direcional configurável via `-A`. |
| **1** | Sunset | Ondas horizontais suaves e quentes. |
| **2** | Phase | Pulso de cor uniforme em todo o texto. |
| **3** | Vertical | Gradiente fixo no eixo Y. |
| **4** | Waves | Ondas senoidais verticais combinadas. |
| **5** | Radial | Expansão circular a partir do centro. |
| **6** | Grid | Padrão de interferência senoidal (X*Y). |
| **7** | H-Spot | Foco de luz centralizado horizontalmente. |
| **8** | V-Spot | Foco de luz centralizado verticalmente. |
| **9** | CRT | Efeito de varredura listrada (Scanlines). |
| **10**| Matrix | Chuva digital com pulsos de brilho aleatórios. |
| **11**| Pulse | Ondas de choque circulares intermitentes. |

</details>

---

## 📦 Instalação

Escolha sua plataforma e siga as instruções para instalar a versão **v2.1.5-STABLE**:

### 🐧 Linux
<details>
<summary>Visualizar instruções</summary>

```bash
# 1. Baixe o pacote x64
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.1.5/neonx_linux-x64.zip

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
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.1.5/neonx_linux-arm64.zip

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
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.1.5/neonx_macos-arm64.zip

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
curl.exe -LO https://github.com/inrryoff/NeonX/releases/download/v2.1.5/neonx_windows-x64.zip

:: 2. Extraia e mova para o System32 (necessário Admin)
tar -xf neonx_windows-x64.zip
move neonx.exe C:\Windows\System32\
```
</details>

<details>
<summary>Visualizar via PowerShell</summary>

```powershell
# 1. Baixe o pacote
curl.exe -LO https://github.com/inrryoff/NeonX/releases/download/v2.1.5/neonx_windows-x64.zip

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
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.1.5/neonx_wasm.zip

# 2. Extraia e rode um servidor local para testar
unzip neonx_wasm.zip
python3 -m http.server 8080
# Acesse http://localhost:8080 no seu navegador
```
</details>

---

## 🔒 Segurança

O NeonX utiliza **Dupla Blindagem**:
1. **Interna:** Auto-verificação Ed25519 em tempo de execução.
   - *Builds Oficiais:* Validadas contra a chave mestre do autor.
   - *Builds Comunitárias/Locais:* O `build.sh` gera automaticamente chaves efêmeras para garantir que o binário seja funcional e íntegro, exibindo o status `VALID_SIG_BY_COMMUNITY`.
2. **Externa:** Assinaturas `.minisig` para verificação de download.

```bash
./verify.sh ./neonx
```

---

## 🤝 Contribuição

Interessado em ajudar? Leia nosso [Guia de Contribuição](./CONTRIBUTING.md).

**NeonX** — Desenvolvido com foco em performance bruta e estética cyberpunk.
Copyright © 2024 @inrryoff.
