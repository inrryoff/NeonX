# 🌈 NeonX - Advanced Terminal Gradient Rendering

**NeonX** é um renderizador de texto de alto desempenho projetado para aplicar gradientes dinâmicos e shaders visuais diretamente no seu terminal (Linux, Android/Termux e Windows). Desenvolvido em C puro e utilizando matemáticas avançadas de ondas senoidais, ele garante extrema velocidade e baixo consumo de CPU, mesmo em modos de animação de 60+ FPS.

![Status: Stable](https://img.shields.io/badge/Status-Stable-brightgreen)
![Version: 1.6.1](https://img.shields.io/badge/Version-1.6.1-blue)
![License](https://img.shields.io/badge/License-NeonX_Custom-purple)

---

## ⚡ Recursos Principais

* **Multiplataforma Nativo:** Funciona perfeitamente em `aarch64` (Termux/Android), `x86_64` e `x86` (Linux e Windows).
* **Shaders Matemáticos:** 12 modos de renderização diferentes, incluindo pulsações, diagonais, e o famoso efeito "Matrix" e "Cyberpunk".
* **Alta Performance:** Escrito em C com tabelas de Lookup (`sin_lut`) pré-calculadas para não engasgar o seu terminal.
* **Stream Mode (`-L`):** Capaz de processar logs dinâmicos em tempo real diretamente do `stdout` (ex: `dmesg | neonx -L -m 10`).
* **Integração com Termux:** Suporte total e otimizado para o ambiente Termux.
* **Segurança de Binário:** Arquitetura anti-tamper (verificação de integridade) embutida via injeção de assinatura no EOF.

---

## 🚀 Como Compilar (Para a Comunidade)

O NeonX utiliza o **Zig** como *toolchain* de cross-compiling. Isso permite que você gere os executáveis para Windows ou Linux estático diretamente do seu celular (via Termux) ou de qualquer outra distribuição.

### 1. Requisitos
Você precisa ter o compilador `zig` instalado. No Termux ou Linux:
```bash
pkg install zig   # Termux
sudo apt install zig # Ubuntu/Debian
```

---

## Gerando builds
Clone este repositório e rode o script de compilação da comunidade:
```bash
git clone https://github.com/inrryoff/NeonX.git
cd NeonX
chmod +x build.sh
./build.sh
```
Um menu interativo será exibido. Basta escolher a sua plataforma e o executável assinado será gerado na pasta /build.

---

## 💻 Como Usar
**O NeonX trabalha interceptando textos via Pipe (|). Você joga a saída de um comando para ele, e ele devolve a arte no terminal.**
Exemplos Básicos:
```bash
# Mostra um arquivo de texto com o gradiente padrão
cat meutexto.txt | neonx

# Modo estático (sem animação) com largura travada em 50 caracteres
cat banner.txt | neonx -S -c 50

# Carrega o preset Cyberpunk
cat neofetch | neonx --preset cyberpunk
```

Argumentos Avançados:
- -m [0-11]: Troca os modos de shader.
- -s [valor]: Velocidade da animação (ex: -s 0.5).
- -f [valor]: Frequência das cores (distância do arco-íris).
- -p [valor]: Define uma "semente" inicial de cor fixa.
- -F [valor]: Trava o FPS da animação (ex: -F 60).

Para ver a lista completa de comandos, digite: `neonx --help`

---

## 🛡️ Notas de Segurança e Licença
Este código é disponibilizado sob a NeonX Custom License. O uso e a modificação para fins pessoais ou para inclusão em módulos gratuitos (como Magisk/KernelSU) são totalmente encorajados. É terminantemente proibida a comercialização deste software.
Aviso de Integridade: As builds compiladas pelo script público se auto-identificarão como GENERIC_COMMUNITY_BUILD. **Apenas as releases oficiais (disponíveis na aba "Releases" do GitHub) carregam a assinatura master original do autor (@inrryoff).**

---

**Engenharia feita com ☕ e foco extremo na linha de comando.**
