# 🌈 NeonX – Colorizador de Texto com Shaders Avançado

[![Versão](https://img.shields.io/badge/version-2.0.1--STABLE-blue)](https://github.com/inrryoff/NeonX/releases)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20Android%20%7C%20Windows-brightgreen)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red)](./LICENSE)

**Dê vida aos seus textos no terminal com 11 shaders animados.** Simples, rápido e totalmente *open source* – agora com arquitetura modular e alta segurança.

---

## ✨ O que é o NeonX?

O **NeonX** é um filtro de cores para o terminal. Você envia um texto via `stdin` e ele retorna o mesmo texto renderizado com animações contínuas de cores – como se fossem *shaders* aplicados a caracteres ASCII. Ideal para deixar logs, banners e saídas de comando visualmente impressionantes.

- **11 modos de animação** (cyberpunk, matrix, pulse, retro, sunset…).
- **Renderização em tempo real** (modo animado ou stream linha a linha).
- **Desempenho máximo** – tabelas de seno pré-calculadas e buffers otimizados em C.
- **Portátil** – binário estático único (Linux, Android/Termux, Windows).
- **Código aberto** – licença que permite modificações e distribuição gratuita, com créditos ao autor original.

---

## 🚀 Uso rápido

```bash
# Exemplo com preset cyberpunk (animado, com duração de 5 segundos)
echo "NeonX" | neonx --preset cyberpunk -d 5

# Modo stream (cada linha é colorida imediatamente)
tail -f log.txt | neonx --preset matrix -L

# Frame estático (-S)
cat banner.txt | neonx -S
```

### 🧰 Opções principais

| Opção | Descrição |
|---|---|
| `-m [0-11]` | Modo de animação. |
| `-s [valor]` | Velocidade da animação. |
| `-f [valor]` | Frequência das cores. |
| `-d [segundos]` | Duração total (0 = infinito). |
| `-S` | Modo estático (congela primeiro frame). |
| `-L` | Modo linha por linha (stream). |
| `--preset [nome]` | Aplica um preset (cyberpunk, retro, matrix, sunset). |
| `-v, --version` | Versão e metadados do binário. |
| `-h, --help` | Esta ajuda. |

*(Use `neonx --help` para ver a lista completa de argumentos).*

---

## 🔒 Integridade e segurança

O NeonX possui um sistema de segurança não destrutivo utilizando **assinaturas Ed25519 (Monocypher)**.

- **Binários oficiais** são compilados e assinados digitalmente.
- **Builds não oficiais** (compiladas localmente ou modificadas) mostram um aviso no `--version` identificando como `MODIFICADO (não oficial)`.
- **Nenhum arquivo é deletado ou alterado** – transparência e controle total na mão do usuário.

---

## 📦 Instalação

### Binários oficiais

Acesse a **[página de releases](https://github.com/inrryoff/NeonX/releases)** e baixe o binário pronto para sua plataforma (Linux, Android/ARM, Windows).

### Compilação local

Para compilar do zero, você precisará de `gcc` ou `clang`.

```bash
git clone https://github.com/inrryoff/NeonX.git
cd NeonX
bash build.sh
```

---

## 📜 Licença

Copyright © 2024 **@inrryoff**.  
Licenciado sob condições especiais – **uso gratuito, modificações permitidas, proibida a venda**.  
Veja o texto completo com `neonx --license`.
