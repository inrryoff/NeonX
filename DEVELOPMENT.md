# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como o NeonX é estruturado e como você pode modificá-lo.

---

## 📁 Estrutura do projeto

```text
NeonX/
├── src/                  # Código-fonte principal
│   ├── main.c            # Ponto de entrada, loop de eventos, parsing
│   ├── integrity.c/.h    # Lógica de validação criptográfica (Ed25519)
│   ├── monocypher.c/.h   # Biblioteca de criptografia leve
│   ├── shaders.c/.h      # Shaders matemáticos e Look-Up Table (LUT)
│   └── terminal.c/.h     # Gerenciamento ANSI e sinais de sistema
├── tools/                # Ferramentas auxiliares
│   ├── keygen.c          # Gera chaves para builds oficiais
│   └── sign_binary.c     # Anexa assinatura ao binário
├── build.sh              # Script genérico para builds locais (devs)
├── release-build.sh      # Pipeline oficial de compilação (usado via Zig CC)
└── build.yml/tests.yml   # Workflows do GitHub Actions
```

---

## 🚀 Compilando como desenvolvedor

Use o script `build.sh` para uma compilação rápida na sua máquina:

```bash
bash build.sh
```

Este script detectará automaticamente o seu compilador (`gcc` ou `clang`) e criará o binário em `./build/neonx`. 

Por padrão, esta compilação será marcada como **`DEVELOPMENT_BUILD`**. O sistema de integridade (em `integrity.c`) identificará que não há uma assinatura Ed25519 válida atrelada e classificará a build como `MODIFICADA`. Isso é o comportamento esperado.

---

## 🔐 Entendendo o sistema de segurança

O NeonX utiliza **Monocypher** para garantir a autenticidade das builds oficiais sem punir modificações da comunidade.

1.  A função `check_integrity()` lê os últimos 128 bytes do binário em execução.
2.  Tenta extrair uma assinatura em hexadecimal (64 bytes originados da chave privada Ed25519).
3.  Usa a `PUBLIC_KEY` fixa no código para validar o hash do arquivo.
4.  Se a validação falhar, o status global muda para `2` (Aviso/Não Oficial).
5.  A função `print_version()` no `terminal.c` exibe os metadados apropriados baseados nesse status.

Nenhum arquivo sofre "autodestruição" caso a assinatura não bata; o código apenas prioriza a transparência com o usuário.

---

## 📝 Testando suas alterações

Após qualquer alteração nos shaders (`shaders.c`) ou rotinas do terminal, valide localmente:

1.  Teste de renderização: `echo "Teste" | ./build/neonx -S -m 10`
2.  Teste de metadados: `./build/neonx --version`
3.  Teste o parser: `./build/neonx --preset sunset -d 2`

Se as mudanças forem estáveis, o fluxo CI configurado no repositório (`tests.yml`) rodará automaticamente nos Pull Requests, incluindo o Valgrind para detecção de Memory Leaks.
