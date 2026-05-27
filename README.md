# 🌈 NeonX – Shaders Avançadas para Terminal

[![Versão](https://img.shields.io/badge/version-2.0.7--STABLE-blue)](https://github.com/inrryoff/NeonX/releases)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Android%20%7C%20Windows-brightgreen)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red)](./LICENSE)
[![Build Oficial](https://img.shields.io/badge/build-oficial-green)]()
[![Testes](https://github.com/inrryoff/NeonX/actions/workflows/tests.yml/badge.svg)](https://github.com/inrryoff/NeonX/actions/workflows/tests.yml)

![Termux](https://img.shields.io/badge/Developed_on-Termux-000000?style=for-the-badge&logo=termux)
![C](https://img.shields.io/badge/C_Puro-00599C?style=for-the-badge&logo=c&logoColor=white)

# 🎨 NeonX — Shader Engine para Terminal

**Dê vida aos seus textos no terminal com 11 shaders animados.** Simples, rápido e totalmente *open source* – agora com verificação de integridade por assinatura digital de ponta a ponta.

---

## ✨ O que é o NeonX?

O **NeonX** é um filtro de cores para o terminal. Você envia um texto via `stdin` e ele retorna o mesmo texto renderizado com animações contínuas de cores – como se fossem *shaders* aplicados a caracteres ASCII. Ideal para deixar logs, banners e saídas de comando visualmente impressionantes.

- **11 modos de animação** (cyberpunk, matrix, pulse, retro, sunset…)
- **Renderização em tempo real** (modo animado ou stream linha a linha)
- **Desempenho máximo** – tabelas de seno pré-calculadas, matemática de ponto fixo e buffers otimizados
- **Portátil** – binário estático único (Linux, macOS, Android, Windows)
- **Código aberto** – licença que permite modificações e distribuição gratuita, com créditos ao autor original
- **Integridade verificável** – Dupla blindagem criptográfica: Auto-verificação interna em C (Ed25519) e externa via Minisign.

---

## 🚀 Uso rápido
<details>
<summary>Ver Exemplo</summary>

```bash
# Exemplo com preset cyberpunk (animado, com duração de 5 segundos)
echo "NeonX" | neonx --preset cyberpunk -d 5
```
```bash
# Modo stream (cada linha é colorida imediatamente)
tail -f log.txt | neonx --preset sunset -L
```
```bash
# Frame estático (-S)
cat banner.txt | neonx -S
```
</details>

### Opções principais
<details>
<summary>Ver todas as opções</summary>

| Opção | Descrição | Padrão |
|---|---|---|
| -m [0-11] | Modo de animação (veja tabela abaixo) | 0 |
| -s [valor] | Velocidade da animação | 0.2 |
| -f [valor] | Frequência das cores (onda senoidal) | 0.3 |
| -d [segundos] | Duração total (0 = infinito) | 0 |
| -A [ângulo] | Define o ângulo do gradiente em graus | 0 - 360 |
| -p [valor] | Fase inicial (seed fixa) | aleatória |
| -S | Modo estático (congela primeiro frame) | desligado |
| -c [largura] | Largura fixa do gradiente | automática |
| -o [0-1] | Opacidade das bordas (vinheta radial) | 0.0 |
| -F [fps] | Taxa de quadros por segundo | 20 (50ms) |
| -L | Modo linha por linha (stream) | desligado |
| --preset [nome] | Aplica um preset (cyberpunk, retro, matrix, sunset) | – |
| --quantized | Ativa o modo quantizado (menor qualidade visual, cores blocadas) | desligado |
| --spin | Exibe paleta de cores crua (ANSI) para uso em scripts externos | – |
| --lang [idioma] | Seleciona o idioma: pt (Português), en (Inglês), es (Espanhol) ou zh (Chinês) | sistema |
| --license | Mostra a licença de uso especial do projeto | – |
| -v, --version | Versão e status de integridade do binário | – |
| -h, --help | Exibe o menu de ajuda | – |

*(Nota: O NeonX possui também uma flag oculta --verify-sig utilizada exclusivamente por scripts de segurança para verificar a validação matemática interna).*
</details>

## 🎨 Modos de animação (shaders)
<details>
<summary>Ver todos os modos</summary>

| Modo | Nome interno | Descrição |
|---|---|---|
| **0** | Gradiente padrão | Gradiente horizontal + vertical com suporte a diagonal (-A) |
| **1** | Sunset (pôr do sol) | Ondas suaves que lembram um céu alaranjado |
| **2** | Fase pura | Apenas o valor da fase, sem variação espacial |
| **3** | Só vertical | Gradiente baseado na posição y |
| **4** | Ondas verticais | Senoide no eixo X combinada com Y, criando ondas |
| **5** | Radial (centro) | Cores a partir da distância do centro da tela |
| **6** | Grade senoidal | Senoides nos dois eixos multiplicadas |
| **7** | Holofote horizontal | Intensidade decrescente do centro para as bordas no eixo X |
| **8** | Holofote vertical | Idem, no eixo Y |
| **9** | Listrado (par/ímpar) | Colunas pares com intensidade alta, ímpares com baixa |
| **10** | Matrix | Efeito chuva de caracteres com pulsos e scanlines |
| **11** | Pulso expansivo | Ondas circulares partindo do centro |
</details>

## 🎯 Presets
Os presets ajustam automaticamente várias opções para um tema específico:
| Preset | Modo | Velocidade | Frequência | Ângulo |
|---|---|---|---|---|
| **cyberpunk** | 0 | 0.3 | 0.5 | 45.0 |
| **retro** | 4 | 0.2 | 0.8 | 0.0 |
| **matrix** | 10 | 0.5 | 1.2 | 90.0 |
| **sunset** | 1 | 0.15 | 0.3 | 30.0 |

* Exemplo de uso combinado:
```bash
cat arquivo.txt | neonx --preset matrix -F 30 -d 5

```
## 🔒 Integridade e Dupla Blindagem
O NeonX adota um sistema de segurança extremo, criando um "beco sem saída" para arquivos modificados através de uma arquitetura de **Dupla Blindagem**:
 1. **Auto-verificação Matemática (Interna):** O executável contém o motor criptográfico Monocypher embutido. Ao rodar o projeto, o próprio código em C cruza seu pacote de dados com a chave pública e uma assinatura Ed25519 de 128 bytes anexada no final do arquivo. Se 1 byte for alterado, o binário acusa o status **MODIFICADO** no comando --version.
 2. **Verificação Minisign (Externa):** As *releases* oficiais possuem um hash registrado na nuvem e uma assinatura externa (.minisig).
> **Verifique a autenticidade do binário baixado:**
> ```bash
> # 1. Baixe o verificador oficial (apenas uma vez)
> curl -O https://raw.githubusercontent.com/inrryoff/NeonX/main/verify.sh
> chmod +x verify.sh
> 
> # 2. Verifique o arquivo (Ele valida Hash + Minisign + Self-Check C)
> ./verify.sh ./neonx
> 
> ```

## 📦 Instalação Rápida (Via Terminal)

A forma mais rápida de instalar o NeonX é baixando e extraindo a release diretamente pelo terminal. 

> [!NOTE]
> Os exemplos abaixo usam as versões mais comuns (`x64` e `arm64`). Se precisar de outra arquitetura, substitua o nome do arquivo no comando `curl` por uma das opções disponíveis:
> * **Linux:** `neonx_linux-x64.zip`, `neonx_linux-arm64.zip`, `neonx_linux-x86.zip`, `neonx_linux-arm32.zip`
> * **macOS:** `neonx_macos-x64.zip`, `neonx_macos-arm64.zip`
> * **Windows:** `neonx_windows-x64.zip`, `neonx_windows-x86.zip`
> * **Native for Cortex-A75:** `neonx_Cortex-A75.zip`

---

### 🐧 Linux

<details>
<summary>Para Linux</summary>
 
```bash
# 1. Baixe o pacote
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.0.5/neonx_linux-x64.zip

# 2. Extraia o binário
unzip neonx_linux-x64.zip

# 3. Mova para o PATH e dê permissão de execução
sudo mv neonx /usr/local/bin/
sudo chmod +x /usr/local/bin/neonx
```
</details>

### 🤖 Android (Termux)
<details>
<summary>Para Android</summary>

```bash
# 1. Baixe o pacote ARM64
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.0.5/neonx_linux-arm64.zip

# 2. Extraia o binário
unzip neonx_linux-arm64.zip

# 3. Mova para o PATH do Termux
mv neonx $PREFIX/bin/
chmod +x $PREFIX/bin/neonx
```
</details>

### 🍏 macOS
<details>
<summary>Para macOS</summary>
 
```bash
# 1. Baixe o pacote (x64 ou arm64)
curl -LO https://github.com/inrryoff/NeonX/releases/download/v2.0.5/neonx_macos-arm64.zip

# 2. Extraia o binário
unzip neonx_macos-arm64.zip

# 3. Instale e remova a proteção de quarentena da Apple
sudo mv neonx /usr/local/bin/
sudo chmod +x /usr/local/bin/neonx
sudo xattr -d com.apple.quarantine /usr/local/bin/neonx
```
</details>

### 🪟 Windows (PowerShell)
*Abra o PowerShell como **Administrador** e rode:*
<details>
<summary>Para Windows</summary>

```powershell
# 1. Baixe o pacote
curl.exe -LO https://github.com/inrryoff/NeonX/releases/download/v2.0.5/neonx_windows-x64.zip

# 2. Extraia o executável
Expand-Archive neonx_windows-x64.zip -DestinationPath . -Force

# 3. Mova para a pasta do sistema para acesso global
Move-Item -Path ".\neonx.exe" -Destination "C:\Windows\System32\" -Force
```
</details>

## 🧑‍💻 Para Desenvolvedores (Build Local)
Quer compilar você mesmo, modificar ou entender a arquitetura?
```bash
git clone https://github.com/inrryoff/NeonX.git
cd NeonX
./build.sh

```
O build.sh utiliza Zig C++ (para Cross-Compiling multiplataforma) ou Clang (Nativo). Ao compilar localmente, o seu binário será totalmente funcional, mas o --version indicará que é uma build local/modificada, já que não contém a chave privada oficial de assinatura.
Leia o **[Guia do Desenvolvedor](./DEVELOPMENT.md)** para mais detalhes sobre a estrutura modular (src/) e a integração da Monocypher.

---

## 📜 Licença
Copyright © 2024 @inrryoff.
Licenciado sob condições especiais – **uso gratuito, modificações permitidas, proibida a venda**.
Veja o texto completo com neonx --license ou no arquivo [LICENSE](./LICENSE).

---


## 📱 História
<details>
<summary>Ver História</summary>
 
**Por que o NeonX existe?**
O clássico lolcat é uma ferramenta histórica que inspirou a customização de terminais no mundo todo. No entanto, ele trazia algumas dores de cabeça estruturais: engasgava com banners gigantes, quebrava arte ASCII complexa em UTF-8, exigia dependências externas (Ruby). O NeonX nasceu para resolver tudo isso, entregando uma ferramenta visual estonteante com foco absoluto em **performance extrema, estabilidade e segurança**.
**Engenharia de Produto com Restrições (100% Mobile)**
Para provar que engenharia de software de verdade não exige setups caros com múltiplos monitores, tenho muito orgulho de dizer que **todo o processo de desenvolvimento do NeonX foi realizado em um smartphone Android, utilizando o Termux e um teclado virtual**.
Desde a primeira linha escrita em C puro, passando pela matemática trigonométrica em ponto fixo dos shaders, a compilação cruzada via Zig, até a implementação de assinaturas criptográficas Ed25519 (Monocypher) e a integração contínua (CI/CD) no GitHub Actions — tudo foi auditado, codificado e testado no bolso.
Engenharia sob fortes restrições gera inovação e pragmatismo. Espero que o NeonX deixe o seu terminal tão incrível quanto foi a jornada de criá-lo! 🚀📱
</details>

