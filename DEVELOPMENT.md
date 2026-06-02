# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como a arquitetura do NeonX é estruturada e como você pode compilar e modificar o código.

---

## 📁 Estrutura do Projeto

A organização de pastas separa o código-fonte dos artefatos de compilação e do material criptográfico.

```text
NeonX/
├── src/                 # Código-fonte principal da Engine
│   ├── main.c           # Ponto de entrada, parsing e loop principal
│   ├── main_wasm.c      # Bridge para WebAssembly
│   ├── build_config.h   # Configurações de build
│   ├── core/            # Núcleo da lógica de processamento
│   │   ├── integrity.c  # Validação de payload
│   │   ├── math_fixed.c # Matemática de ponto fixo
│   │   ├── monocypher.c # Motor de criptografia
│   │   ├── msgs.c       # Internacionalização
│   │   ├── render.c     # Abstrações de renderização
│   │   ├── render_core.c# Motor de renderização core
│   │   ├── shader_effects.c # Efeitos de shader
│   │   ├── shaders.c    # Gerenciamento de presets
│   │   └── terminal.c   # Gerenciamento ANSI
│   └── headers/         # Cabeçalhos e definições de API
│       ├── neonx.h      # API Pública Unificada
│       └── ...          # Outros cabeçalhos de suporte
├── tools/               # Ferramentas auxiliares (incluindo gerador de chaves)
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
