# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como a arquitetura do NeonX é estruturada e como você pode compilar e modificar o código.

---

## 📁 Estrutura do Projeto

A organização de pastas separa de forma modular as implementações, as interfaces públicas/privadas e os artefatos de build.

```text
NeonX/
├── core/                # Núcleo da lógica de processamento (.c)
│   ├── integrity.c      # Validação de payload e assinatura digital
│   ├── math_fixed.c     # Matemática procedural de ponto fixo e LUTs
│   ├── monocypher.c     # Motor de criptografia (EdDSA/Ed25519)
│   ├── msgs.c           # Tabelas de internacionalização (i18n)
│   ├── render.c         # Gerenciamento de buffers de renderização TTY
│   ├── render_core.c    # Algoritmos core de desenho e cor de pixel
│   ├── shader_effects.c # Fórmulas matemáticas de efeitos de shader
│   ├── shaders.c        # Configurações de presets e mapeamento de offsets
│   └── terminal.c       # Manipulação de estado e escapes ANSI do terminal
├── headers/             # Cabeçalhos, APIs e definições de dados (.h)
│   ├── build_config.h   # Identificadores de build e SyncID (Dinâmico)
│   ├── neonx.h          # API Pública Unificada da Engine
│   └── ...              # Cabeçalhos internos de suporte do core
├── tools/               # Ferramentas auxiliares (keygen, assinador, etc.)
├── build/               # Artefatos e binários finais compilados
├── main.c               # Ponto de entrada CLI, parser de flags e loop nativo
├── main_wasm.c          # Ponte e bindings Emscripten para WebAssembly
├── ARCHITECTURE.md      # Explicação detalhada dos fluxos de dados do motor
├── Makefile             # Gerenciador estruturado de compilação (Clang/Make)
├── build.sh             # Script automatizado para compilações locais e suíte de testes
├── CHANGELOG.md # Registro de alterações ao longo das versões
└── README.md            # Documentação e instruções gerais de uso

```
## 📦 Compilação
O projeto utiliza (builds).sh e um (Makefile básico) para gerenciar as builds.

### Build (Com Makefile)
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
## 🚀 Via build.sh (Recomendado)
```bash
./build.sh
# E siga o menu conforme necessário
```

### Para testar
```bash
./build.sh --test
```
### Comandos disponíveis para CI/CD
| Flag | uso |
| ------ | ----- |
| --native | Compilação rápida para seu sistema própio (via clang ou MSVC (No windows). |
| --wasm | Compilação rápida para WebAssembly (wasm). |
| --test | Faz fuzzing leve para verificar se o binário está ok se falhar em qualquer etapa o binário não é apto para distribuição. | 

## 📦 Usando a biblioteca NeonX (Core)
A partir da v2.2.3, o NeonX oferece o cabeçalho headers/neonx.h como interface única para integração.
### Integração Rápida
Basta incluir o cabeçalho no seu código C/C++ (garantindo o mapeamento da pasta via -I no compilador):
```c
#include "neonx.h"

```
### Compilação da Biblioteca Estática
O Makefile gera automaticamente a biblioteca estática:
```bash
# Isso gera o arquivo build/libneonx_core.a
make

```
## 🌐 WebAssembly (Uso na Web)
O motor de cores do NeonX roda no navegador via WASM. A arquitetura é baseada no RenderDriver (headers/render_driver.h), o que permite que a mesma lógica de renderização procedural seja utilizada tanto no CLI quanto na Web.
A versão WASM agora também utiliza a API unificada para garantir paridade total de recursos.
## 🏗️ Performance e Otimizações
Se você estiver contribuindo para o núcleo (core), atente-se ao sistema de **Length Caching** na estrutura Content. Todas as strings de entrada têm seu comprimento pré-calculado em load_input_data para evitar o custo de wcslen durante o loop de animação de alta frequência.
```
