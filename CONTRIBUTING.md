# Guia de Contribuição - NeonX 🌈

Olá! Ficamos muito felizes que você queira ajudar o **NeonX**. Não importa se você é um desenvolvedor experiente ou alguém que quer apenas apoiar o projeto, há várias formas de contribuir!

---

## 💖 Como você pode ajudar?

### 1. Sendo um Apoiador (Doações)
Se você gosta do projeto and quer ajudar a manter os servidores ou o café do desenvolvedor, você pode fazer uma doação! Isso nos ajuda a dedicar mais tempo a melhorias e novos recursos.
- **Link para Doação:** [Clique aqui para apoiar](https://tipa.ai/TIPARYITV51W4R3SLEG22KVVK9I1)

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
Gera o binário em build/neonx.
### Testes
Sempre rode os testes antes de enviar uma alteração:
```bash
./build.sh --test

```
### Padrões de Código
 * **Linguagem:** C puro (C99/C11).
 * **Arquitetura:** Leia o ARCHITECTURE.md antes de modificar o núcleo.
 * **Matemática:** Use as macros de ponto fixo em headers/math_fixed.h. **Não utilize float ou double**.
 * **API:** Utilize sempre o cabeçalho mestre headers/neonx.h para expor novas funcionalidades.
 * **Performance:** Respeite o sistema de **Length Caching**. Evite chamadas a wcslen dentro de loops de renderização.
 * **Commits:** Seja claro no que mudou (ex: feat: adiciona preset ocean, fix: corrige bug no parser).
## 🔒 Assinatura e Integridade
O NeonX utiliza uma arquitetura de verificação de payload para garantir que o binário não foi alterado. O script build.sh agora gerencia isso automaticamente: ele gera chaves efêmeras para assinatura se nenhuma for encontrada na pasta keys/. Você não precisa se preocupar com isso para testes de desenvolvimento, a menos que esteja criando um fork oficial com sua própria infraestrutura de distribuição.
**Obrigado por fazer parte da comunidade NeonX!** 🚀
```