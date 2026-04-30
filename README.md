# 🌈 NeonX – Colorizador de Texto com Shaders avançado

[![Versão](https://img.shields.io/badge/version-2.0.0--STABLE-blue)](https://github.com/inrryoff/NeonX/releases)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20Android%20%7C%20Windows-brightgreen)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red)](./LICENSE)
[![Build Oficial](https://img.shields.io/badge/build-oficial-green)](./build-oficial.sh)
[![Build Comunidade](https://img.shields.io/badge/build-comunidade-orange)](./build-community.sh)

**NeonX** é um motor de renderização de terminal que aplica efeitos de cor dinâmicos (shaders) a qualquer texto recebido via `stdin`. Funciona como um filtro de cores: você envia texto (código, logs, arte ASCII) e o NeonX devolve o mesmo conteúdo colorizado com animações contínuas, estáticas ou em tempo real (modo stream).

---

## ✨ Características principais

- 🎨 **11 modos de animação** (shaders) – de gradientes suaves a efeitos matrix e pulsos
- 🧠 **Renderização otimizada** – tabela de seno pré-calculada e buffer de saída
- 🌐 **Suporte a texto Unicode (wide chars)**
- 🖥️ **Multi-plataforma** – Linux, Android (Termux) e Windows (com ANSI habilitado)
- 📦 **Binário estático único** (via Zig + musl) – sem dependências externas
- 🔐 **Verificação de integridade criptográfica** – impede execução de binários modificados
- 🏷️ **Presets de fábrica**: `cyberpunk`, `retro`, `matrix`, `sunset`
- ⚙️ **Controle preciso**: velocidade, frequência de cor, FPS, opacidade, duração, largura fixa

---

## 📥 Uso básico

```bash
cat arquivo.txt | neonx

```
Ou com opções:
```bash
cat logo.txt | neonx -d 5 -m 1 -s 0.15 -f 0.3 --preset sunset -F 60 -o 0

```

---

## 🧰 Opções de linha de comando
| Opção | Descrição | Padrão |
|---|---|---|
| -m [0-11] | Modo de animação (veja tabela abaixo) | 0 |
| -s [valor] | Velocidade da animação | 0.2 |
| -f [valor] | Frequência das cores (onda senoidal) | 0.3 |
| -d [segundos] | Duração total (0 = infinito) | 0 |
| -D [valor] | Inclinação diagonal do gradiente | 0.0 |
| -p [valor] | Fase inicial (seed fixa) | aleatória |
| -S | Modo estático (congela primeiro frame) | desligado |
| -c [largura] | Largura fixa do gradiente | automática |
| -o [0-1] | Opacidade das bordas (vinheta radial) | 1.0 |
| -F [fps] | Taxa de quadros por segundo | 20 (50ms) |
| -L | Modo linha por linha (stream) | desligado |
| --preset [nome] | Aplica um preset (cyberpunk, retro, matrix, sunset) | – |
| --spin | Exibe paleta de cores em formato 38;2;R;G;B para uso externo | – |
| --license | Mostra a licença de uso | – |
| -v, --version | Versão e metadados do binário | – |
| -h, --help | Esta ajuda | – |

---

## 🎨 Modos de animação (shaders)
| Modo | Nome interno | Descrição |
|---|---|---|
| 0 | Gradiente padrão | Gradiente horizontal + vertical com suporte a diagonal (-D) |
| 1 | Sunset (pôr do sol) | Ondas suaves que lembram um céu alaranjado |
| 2 | Fase pura | Apenas o valor da fase, sem variação espacial |
| 3 | Só vertical | Gradiente baseado na posição y |
| 4 | Ondas verticais | Senoide no eixo X combinada com Y, criando ondas |
| 5 | Radial (centro) | Cores a partir da distância do centro da tela |
| 6 | Grade senoidal | Senoides nos dois eixos multiplicadas |
| 7 | Holofote horizontal | Intensidade decrescente do centro para as bordas no eixo X |
| 8 | Holofote vertical | Idem, no eixo Y |
| 9 | Listrado (par/ímpar) | Colunas pares com intensidade alta, ímpares com baixa |
| 10 | Matrix | Efeito chuva de caracteres com pulsos e scanlines |
| 11 | Pulso expansivo | Ondas circulares partindo do centro |

---

## 🎯 Presets
Os presets ajustam automaticamente várias opções para um tema específico:
| Preset | Modo | Velocidade | Frequência | Diagonal |
|---|---|---|---|---|
| cyberpunk | 0 | 0.3 | 0.5 | 0.1 |
| retro | 4 | 0.2 | 0.8 | 0.0 |
| matrix | 10 | 0.5 | 1.2 | 0.0 |
| sunset | 1 | 0.15 | 0.3 | 0.05 |
Exemplo:
```bash
cat arquivo | neonx --preset matrix -F 30

```

---

## 🔐 Integridade do binário
O NeonX verifica sua própria integridade em cada execução. Um selo criptográfico (FNV-1a + cifra XOR com chave secreta) é injetado durante o build oficial. Se o binário for modificado, ele se autodestrói e exibe:
```text
[FATAL] Integridade do NeonX violada. Binario corrompido.

```
### ✅ Verificando um binário oficial
Use o script verify_public.sh para confirmar a origem:
```bash
bash verify_public.sh neonx

```
O script baixa o registro de hashes oficiais do GitHub e compara com o arquivo fornecido – **independentemente do nome do arquivo**.

---

## 🧪 Exemplos criativos
```bash
# Efeito arco-íris sobre um logo
figlet "NeonX" | neonx -m 0 -s 0.3 -f 0.8 -D 0.1

# Modo Matrix com texto de um arquivo
cat hack.txt | neonx --preset matrix -F 15

# Congelar uma imagem colorida (estático)
cat arte.txt | neonx -m 5 -S -p 1.5

# Stream contínuo (monitoramento de logs com cores)
tail -f /var/log/syslog | neonx -L -m 3 -s 0.1

```

---

## 🏗️ Compilação
Um script de build é fornecido:
 * **build-community.sh** – gera binários genéricos para a comunidade (chave pública GENERIC_KEY_UNOFFICIAL)
Cria automaticamente pacotes .zip com o binário renomeado para neonx (ou neonx.exe), prontos para distribuição.

---

## 🤝 Contribuição
Pull requests são bem-vindos, desde que mantenham os créditos e a licença original.
Para bugs ou sugestões, abra uma issue.

---

## 📄 Licença
NeonX é distribuído sob a **NeonX Custom License**.
Veja o texto completo com neonx --license ou no arquivo LICENSE.
- **Autor:** @inrryoff
- **Versão:** 2.0.0-STABLE
- **Plataformas:** Linux • Android • Windows
