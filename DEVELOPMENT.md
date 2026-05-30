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
│   └── style.h          # Estilos predefinidos e a logo do NeonX
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

---

## 🌐 WebAssembly (Uso na Web)

O motor de cores do NeonX roda no navegador via WASM. O driver de renderização abstraído permite que a mesma lógica de colorização ANSI seja utilizada via WebAssembly.

---

## 📝 Testando suas Alterações
Após qualquer alteração, valide o sistema:
```bash
./build.sh --test
```

---

## 🔱 Criando um Fork e Personalizando

Se você deseja criar sua própria versão do NeonX:

1.  **Fork o Repositório:** Use o botão de Fork no GitHub.
2.  **Personalize a Identidade:** No arquivo `src/style.h`, você pode alterar o `LOGO_NEONX` e as cores padrão.
3.  **Adicione seus Presets:** Modifique `src/shaders.c` para incluir novas paletas e comportamentos.
4.  **Assine seu Binário:** Siga as instruções em `CONTRIBUTING.md` para gerar suas próprias chaves e garantir que seu binário seja reconhecido como oficial do seu fork.
5.  **Mantenha os Créditos:** Lembre-se que a licença exige a manutenção dos créditos ao autor original (@inrryoff).

---
Desenvolvido com ☕, C e Termux.
