# 🌈 NeonX – Ultra-High Performance Terminal Colorizer

[![Versão](https://img.shields.io/badge/version-2.2.1--STABLE-blue)](https://github.com/inrryoff/NeonX/releases)
[![NeonX Web](https://img.shields.io/badge/NeonX-Web_Demo-0a0a0f?style=flat-square&logo=terminal&logoColor=00ffcc)](https://neonx-web.netlify.app/)
[![Plataformas](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Android%20%7C%20Windows%20%7C%20WASM-brightgreen)]()
[![Licença](https://img.shields.io/badge/license-NeonX%20Custom-red)](./LICENSE)

![C](https://img.shields.io/badge/C_Puro-00599C?style=for-the-badge&logo=c&logoColor=white)
![WASM](https://img.shields.io/badge/WebAssembly-654FF0?style=for-the-badge&logo=webassembly&logoColor=white)

**NeonX** é um colorizador de terminal de altíssima performance escrito em C puro. Utilizando matemática de ponto fixo e shaders procedurais, ele transforma textos estáticos em animações fluidas diretamente no seu terminal ou navegador.

---

## 🚀 Início Rápido

```bash
# 1. Pipe simples (Animação padrão)
cat banner.txt | neonx

# 2. Logs em tempo real (Modo Stream) com preset específico
tail -f access.log | neonx -L --preset dracula

# 3. Animação estática de alta qualidade
echo "NeonX Engine" | neonx --preset synthwave -S
```

---

## ✨ O que há de novo na v2.2.1-STABLE

- **Paletas de Cores Reais:** Presets como `dracula`, `synthwave` e `hacker` agora utilizam canais RGB específicos para fidelidade visual absoluta.
- **7 Novos Temas:** `vaporwave`, `ocean`, `forest`, `blood`, `hacker`, `synthwave` e `dracula`.
- **Parser Blindado:** Correção de bugs críticos no parsing de argumentos que causavam comportamento inesperado ao combinar flags.
- **Performance de Elite:** Matemática Fixed-Point otimizada para CPUs modernas e sistemas embarcados.

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
| `--preset` | Presets: `cyberpunk`, `retro`, `matrix`, `sunset`, `dracula`, `hacker`, etc. | - |
| `--quantized`| Ativa modo 15-bit (retrô/blocos) | OFF |
| `--lang [id]` | Seleciona o idioma: `pt`, `en`, `es`, `zh`, `ja`, etc. | auto |

</details>

---

## 🎨 Presets e Shaders

O NeonX agora conta com um sistema de paletas customizadas:

- **Hacker:** O clássico verde Matrix puro.
- **Dracula:** Tons sombrios de roxo, rosa e ciano.
- **Synthwave:** Estética oitentista com rosa neon e azul.
- **Vaporwave:** Gradientes suaves e relaxantes.
- **Matrix / Cyberpunk / Retro / Sunset** e muito mais!

---

## 💖 Apoie o Projeto

Se o **NeonX** é útil para você ou para sua equipe, considere apoiar o desenvolvimento contínuo!

- **Doações:** [Clique aqui para apoiar via PayPal/Pix/etc] (Substitua pelo seu link!)
- **Siga o autor:** [@inrryoff](https://github.com/inrryoff)
- **Dê uma estrela:** ⭐ No GitHub para ajudar na visibilidade!

---

## 🌍 Internacionalização

O NeonX detecta automaticamente o idioma do seu sistema ou pode ser forçado:
```bash
neonx --lang en --help
```

---
Desenvolvido com ☕ e C.
