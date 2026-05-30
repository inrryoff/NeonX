# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como a arquitetura do NeonX é estruturada e como você pode compilar e modificar o código.

---

## 📁 Estrutura do Projeto

A organização de pastas separa o código-fonte dos artefatos de compilação e do material criptográfico.

```text
NeonX/
├── src/                 # Código-fonte principal da Engine
│   ├── main.c           # Ponto de entrada, parsing e loop principal
│   ├── math_fixed.c     # Matemática de ponto fixo e entropia segura
│   ├── shader_effects.c # Lógica interna dos efeitos de shader
│   ├── render_core.c    # Motor de renderização e abstração de cores
│   ├── render_driver.h  # Interface agnóstica para drivers de saída
│   ├── integrity.c/.h   # Lógica de validação criptográfica (Ed25519)
│   ├── monocypher.c/.h  # Motor de criptografia leve
│   ├── shaders.c/.h     # Gerenciamento de presets (agora via JSON)
│   ├── terminal.c/.h    # Gerenciamento ANSI e sinais de sistema
│   └── msgs.c/.h        # Sistema i18n (agora via JSON)
├── assets/              # Recursos externos (JSON)
│   ├── presets.json     # Configurações de efeitos
│   └── i18n.json        # Traduções dinâmicas
├── tools/               # Ferramentas auxiliares
├── build/               # Binários compilados
├── Makefile             # Sistema de build estruturado (Clang/Make)
├── build.sh             # Script rápido para builds e testes
└── README.md            # Documentação principal
```

## 🚀 Compilação

O projeto agora utiliza um `Makefile` para gerenciar as builds.

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

O NeonX expõe seu núcleo como módulos C independentes.

### Compilação da Biblioteca
Para gerar a biblioteca estática manualmente:
```bash
clang -O3 -c src/math_fixed.c -o build/math_fixed.o
clang -O3 -c src/shader_effects.c -o build/shader_effects.o
clang -O3 -c src/render_core.c -o build/render_core.o
ar rcs build/libneonx_core.a build/math_fixed.o build/shader_effects.o build/render_core.o
```

### Exemplo de Uso em C
```c
#include "render_core.h"
#include "math_fixed.h"
#include <stdio.h>

int main() {
    neonx_init_lut(); 
    int r, g, b;
    neonx_get_color(10 << 16, 10 << 16, 1, 0, 0, 0, 12345, &r, &g, &b);
    printf("Cor: R:%d G:%d B:%d\n", r, g, b);
    return 0;
}
```

---

## 🌐 WebAssembly (Uso na Web)

O motor de cores do NeonX roda no navegador via WASM. O build (`make wasm`) gera `neonx.js` e `neonx.wasm`.

O driver de renderização abstraído permite que a mesma lógica de colorização ANSI seja utilizada via WebAssembly em aplicações web que emulem terminais ou via canvas.

---

## 📝 Testando suas Alterações
Após qualquer alteração, valide o sistema:
```bash
# Teste unitário e de integração padrão
./build.sh --test

# Teste unitário verboso (Debug de precisão e overflows)
./build.sh --test-debug
```
Ou manualmente:
```bash
cat assets/i18n.json | ./build/neonx --preset sunset -L
```
