# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como a arquitetura do NeonX é estruturada e como você pode compilar e modificar o código.

---

## 📁 Estrutura do Projeto

A organização de pastas separa o código-fonte dos artefatos de compilação e do material criptográfico.

```text
NeonX/
├── src/                 # Código-fonte principal da Engine
│   ├── neonx.h          # API Pública Unificada (Cabeçalho mestre)
│   ├── main.c           # Ponto de entrada, parsing e loop principal
│   ├── math_fixed.c     # Matemática de ponto fixo e entropia segura
│   ├── shader_effects.c # Lógica interna dos efeitos de shader
│   ├── render_core.c    # Motor de renderização e abstração de cores
│   ├── render_driver.h  # Interface agnóstica para drivers de saída
│   ├── integrity.c/.h   # Lógica de validação criptográfica (Ed25519)
│   ├── monocypher.c/.h  # Motor de criptografia leve
│   ├── shaders.c/.h     # Gerenciamento de presets
│   ├── terminal.c/.h    # Gerenciamento ANSI e sinais de sistema
│   └── style.h          # Estilos predefinidos e a logo do NeonX
├── tools/               # Ferramentas auxiliares
├── build/               # Binários compilados
├── ARCHITECTURE.md      # Explicação detalhada da estrutura da engine
├── Makefile             # Sistema de build estruturado (Clang/Make)
├── build.sh             # Script rápido para builds e testes
└── README.md            # Documentação principal
```

## 🚀 Compilação

O projeto utiliza um `Makefile` para gerenciar as builds.

### Build de Release (Padrão)
```bash
make
```

### Build de Debug
```bash
make debug
```

### WebAssembly (WASM)
```bash
make wasm
```

---

## 📦 Usando a biblioteca NeonX (Core)

A partir da v2.2.3, o NeonX oferece o cabeçalho `src/neonx.h` como interface única para integração.

### Integração Rápida
Basta incluir o cabeçalho no seu código C/C++:
```c
#include "neonx.h"
```

### Compilação da Biblioteca Estática
O `Makefile` gera automaticamente a biblioteca estática:
```bash
# Isso gera o arquivo build/libneonx_core.a
make
```

---

## 🌐 WebAssembly (Uso na Web)

O motor de cores do NeonX roda no navegador via WASM. A arquitetura é baseada no `RenderDriver` (`src/render_driver.h`), o que permite que a mesma lógica de renderização procedural seja utilizada tanto no CLI quanto na Web.

A versão WASM agora também utiliza a API unificada para garantir paridade total de recursos.

---

## 🏗️ Performance e Otimizações
Se você estiver contribuindo para o núcleo (core), atente-se ao sistema de **Length Caching** na estrutura `Content`. Todas as strings de entrada têm seu comprimento pré-calculado em `load_input_data` para evitar o custo de `wcslen` durante o loop de animação de alta frequência.
