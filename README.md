# 🌈 NeonX – Colorizador de Texto com Shaders avançado

[![Versão](https://img.shields.io/badge/version-2.0.0--STABLE-blue)](https://github.com/inrryoff/NeonX/releases)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20Android%20%7C%20Windows-brightgreen)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red)](./LICENSE)
[![Build Oficial](https://img.shields.io/badge/build-oficial-green)]()
[![Build Comunidade](https://img.shields.io/badge/build-comunidade-orange)](./build.sh)[![NeonX Build](https://github.com/inrryoff/NeonX/actions/workflows/build.yml/badge.svg)](https://github.com/inrryoff/NeonX/actions)[![NeonX Test](https://github.com/inrryoff/NeonX/actions/workflows/tests.yml/badge.svg)](https://github.com/inrryoff/NeonX/actions)


# 🎨 NeonX — Shader Engine para Terminal

**Dê vida aos seus textos no terminal com 11 shaders animados.**  
Simples, rápido e totalmente *open source* – agora mais seguro e modular.

---

## ✨ O que é o NeonX?

O **NeonX** é um filtro de cores para o terminal. Você envia um texto via `stdin` e ele retorna o mesmo texto renderizado com animações contínuas de cores – como se fossem *shaders* aplicados a caracteres ASCII. Ideal para deixar logs, banners e saídas de comando visualmente impressionantes.

- **11 modos de animação** (cyberpunk, matrix, pulse, retro, sunset…)
- **Renderização em tempo real** (modo animado ou stream linha a linha)
- **Desempenho máximo** – tabelas de seno pré-calculadas e buffers otimizados
- **Portátil** – binário estático único (Linux, Android, Windows)
- **Código aberto** – licença que permite modificações e distribuição gratuita, com créditos ao autor original

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

### Opções principais
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
cat arquivo | neonx --preset matrix -F 30 -d 5

```

---

## 🔒 Integridade e segurança

O NeonX possui um sistema **não destrutivo** de verificação de integridade:

- **Binários oficiais** são compilados e assinados por **@inrryoff** e não exibem aviso.
- **Builds não oficiais** (compiladas localmente, modificadas ou de terceiros) mostram um aviso amarelo **sem bloquear a execução**.
- A flag `--allow-mod` omite esse aviso e fica memorizada para aquele binário (cache em `~/.neonx_cache/`).
- Nenhum arquivo é deletado ou alterado – **transparência total**.

> **Verifique a autenticidade de um binário baixado** com nosso verificador público:
> ```bash
> curl -O https://raw.githubusercontent.com/inrryoff/NeonX/main/verified_public.sh
> bash verified_public.sh ./neonx
> ```

---

## 📦 Obtendo o NeonX

### Binários oficiais

Acesse a **[página de releases](https://github.com/inrryoff/NeonX/releases)** e baixe o binário para sua plataforma:
- Android /ARM64 /ARM32 (via Termux)
- Linux x86_64 / ARM64 / ARM32
- Windows x64 / x86

Cada release inclui os hashes para verificação manual.

### Compilando você mesmo (build local)

```bash
git clone https://github.com/inrryoff/NeonX.git
cd NeonX
bash build.sh
```

Será gerado um binário `build/neonx` (não oficial) que você pode usar livremente.

---

## 🧑‍💻 Para desenvolvedores

Quer modificar, adicionar shaders ou entender a arquitetura?  
Leia o **[Guia do Desenvolvedor](DEVELOPMENT.md)** para:
- Estrutura modular do código (`src/`)
- Como usar `build.sh` e submeter modificações para aprovação
- Funcionamento do selo criptográfico e do cache de integridade

---

## 📜 Licença

Copyright © 2024 [@inrryoff](https://github.com/inrryoff).  
Licenciado sob condições especiais – **uso gratuito, modificações permitidas, proibida a venda**.  
Veja o texto completo com `neonx --license` ou no arquivo [LICENSE](./LICENSE).

---

**Divirta-se colorindo seu terminal!** 🌈
