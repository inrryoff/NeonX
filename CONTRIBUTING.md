# Guia de Contribuição - NeonX 🌈

Obrigado por se interessar em contribuir para o NeonX! Este é um projeto focado em performance extrema e estética para o terminal e web.

## 🛠️ Como Compilar Localmente

O NeonX utiliza o script `build.sh` para gerenciar todo o ciclo de vida do build:

1.  **Build Nativa:**
    ```bash
    ./build.sh --native
    ```
    Gera o binário em `build/neonx`. Use `PORTABLE=1` para máxima compatibilidade com CPUs antigas.

2.  **WebAssembly (WASM):**
    Para compilar a versão web, você precisará do **Emscripten**.
    - No **Termux**: `pkg install emscripten`
    - No Linux: `sudo apt install emscripten` (ou via SDK oficial).
    
    Rode o comando:
    ```bash
    ./build.sh # Escolha a Opção 2 no menu
    ```
    *Aviso: Não utilize caminhos absolutos (hardcoded) em scripts ou código novo para garantir a portabilidade entre ambientes.*

3.  **Cross-Compilation:**
    Utilizamos Zig para gerar binários para Windows, macOS e diversas arquiteturas Linux. Basta rodar `./build.sh` e seguir o menu.

## 🧪 Testes e Validação

Antes de abrir um Pull Request, você **deve** validar suas alterações:

1.  **Testes Unitários:**
    ```bash
    ./build.sh --test
    ```
    Isso valida a integridade da matemática de ponto fixo e funções do core.

2.  **Interface WASM:**
    Se você alterou o `neonx_core.c` ou `shaders.c`, teste a interface web abrindo o arquivo `index.html` em um servidor local (ex: `python3 -m http.server`) após compilar o WASM. Como nosso CI atual foca em builds nativas, a validação manual do WASM é obrigatória.

## 📝 Padrões de Código

*   **Linguagem:** C puro (C99/C11).
*   **Matemática:** Use sempre as macros de ponto fixo em `neonx_core.h`. **Não utilize `float` ou `double`** no motor de renderização principal.
*   **Commits:** Siga o padrão [Conventional Commits](https://www.conventionalcommits.org/) (ex: `feat:`, `fix:`, `docs:`).

## 🔒 Política de Segurança

*   O NeonX possui auto-verificação de integridade. Se você modificar o código, o binário indicará status `MODIFIED`. Isso é esperado para builds da comunidade.
*   **Nunca** desative ou remova as checagens de integridade no `integrity.c`.

---
Desenvolvido com ☕, C e Termux.
