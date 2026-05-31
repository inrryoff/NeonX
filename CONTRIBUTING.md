# Guia de Contribuição - NeonX 🌈

Olá! Ficamos muito felizes que você queira ajudar o **NeonX**. Não importa se você é um desenvolvedor experiente ou alguém que quer apenas apoiar o projeto, há várias formas de contribuir!

---

## 💖 Como você pode ajudar?

### 1. Sendo um Apoiador (Doações)
Se você gosta do projeto e quer ajudar a manter os servidores ou o café do desenvolvedor, você pode fazer uma doação! Isso nos ajuda a dedicar mais tempo a melhorias e novos recursos.
- **Link para Doação:** [Clique aqui para apoiar via PayPal/Pix/etc]

### 2. Reportando Bugs e Sugerindo Ideias
Encontrou algo errado ou tem uma ideia para um novo preset? Abra uma **Issue** no GitHub! Descreva o que aconteceu e como podemos melhorar.

### 3. Melhorando a Documentação
Viu um erro de português ou algo difícil de entender? Sinta-se à vontade para enviar um Pull Request corrigindo os arquivos `.md`.

---

## 🛠️ Guia para Desenvolvedores

Se você quer colocar a mão no código, aqui está o caminho das pedras:

### Compilação Local
O NeonX utiliza o script `build.sh` para facilitar tudo:
```bash
./build.sh --native
```
Gera o binário em `build/neonx`.

### Testes
Sempre rode os testes antes de enviar uma alteração:
```bash
./build.sh --test
```

### Padrões de Código
*   **Linguagem:** C puro (C99/C11).
*   **Arquitetura:** Leia o [ARCHITECTURE.md](./ARCHITECTURE.md) antes de modificar o núcleo.
*   **Matemática:** Use as macros de ponto fixo em `math_fixed.h`. **Não utilize `float` ou `double`**.
*   **API:** Utilize sempre o cabeçalho mestre `src/neonx.h` para expor novas funcionalidades.
*   **Performance:** Respeite o sistema de **Length Caching**. Evite chamadas a `wcslen` dentro de loops de renderização.
*   **Commits:** Seja claro no que mudou (ex: `feat: adiciona preset ocean`, `fix: corrige bug no parser`).

---

## 🔒 Assinatura e Integridade

Para que o NeonX aceite seu binário como seguro, ele precisa ser assinado. O script `build.sh` faz isso automaticamente com uma chave temporária para testes locais. Se você quiser se tornar um mantenedor oficial ou criar um fork persistente, consulte a seção de assinaturas no `DEVELOPMENT.md`.

---
**Obrigado por fazer parte da comunidade NeonX!** 🚀
