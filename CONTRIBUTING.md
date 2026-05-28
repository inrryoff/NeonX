# Guia de Contribuição - NeonX 🌈

Obrigado por se interessar em contribuir para o NeonX! Este é um projeto open source focado em performance e estética para o terminal.

## 🛠️ Como Compilar Localmente

O NeonX utiliza scripts Bash para facilitar o processo de build:

1.  **Build Nativa:**
    ```bash
    ./build.sh --native
    ```
    Isso gerará o binário em `build/neonx`.

2.  **Build Portátil:**
    Se você deseja gerar um binário compatível com CPUs mais antigas:
    ```bash
    PORTABLE=1 ./build.sh --native
    ```

3.  **Cross-Compilation:**
    O projeto utiliza Zig para compilar para diversas plataformas. Basta rodar `./build.sh` sem argumentos para ver o menu interativo.

## 🧪 Testes

Sempre verifique se suas alterações não quebraram a lógica matemática ou o motor de renderização:

```bash
./build.sh --test
```

## 📝 Padrões de Código e Commits

*   **Linguagem:** C puro (C99/C11) com foco em portabilidade.
*   **Estilo:** Mantenha a consistência com o código existente (uso de ponto fixo para matemática, nomes claros de funções).
*   **Mensagens de Commit:** Recomendamos o uso de [Conventional Commits](https://www.conventionalcommits.org/):
    *   `feat:` Nova funcionalidade.
    *   `fix:` Correção de bug.
    *   `docs:` Alterações na documentação.
    *   `style:` Formatação, ponto e vírgula ausente, etc.
    *   `refactor:` Refatoração de código que não altera comportamento.

## 🔒 Política de Pull Requests

*   Commits assinados (GPG/SSH/Ed25519) são bem-vindos e incentivados, mas não obrigatórios.
*   Certifique-se de que a build passa em todos os jobs do CI (Linux, macOS, Windows).
*   **Importante:** Nunca introduza código que bloqueie a execução do binário por falha de integridade. O NeonX deve apenas avisar sobre modificações, permanecendo funcional.

---
Desenvolvido com ☕ e Termux.
