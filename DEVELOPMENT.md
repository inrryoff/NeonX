# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como a arquitetura do NeonX é estruturada, como a dupla blindagem criptográfica funciona por debaixo dos panos e como você pode compilar e modificar o código.

---

## 📁 Estrutura do Projeto

A organização de pastas foi desenhada para separar o código-fonte dos artefatos de compilação e do material criptográfico.

```text
NeonX/
├── src/                 # Código-fonte principal da Engine
│   ├── main.c           # Ponto de entrada, loop de renderização fundido, parsing
│   ├── integrity.c/.h   # Lógica de validação criptográfica em C (Ed25519)
│   ├── monocypher.c/.h  # Motor de criptografia leve e de alta performance
│   ├── shaders.c/.h     # Aritmética de ponto fixo (Q16.16) e Look-Up Tables
│   ├── terminal.c/.h    # Gerenciamento ANSI, buffers e sinais de sistema (SIGINT)
│   └── msgs.c/.h        # Sistema de internacionalização (PT/EN)
├── tools/               # Ferramentas auxiliares para o Host (Build-time)
│   ├── keygen.c         # Gera o par de chaves (NeonX.key e NeonX.pub)
│   └── sign_binary.c    # Anexa a assinatura de 128 bytes no final do binário
├── keys/                # Chaves criptográficas, hashes e assinaturas
│   ├── NeonX.key / .pub # Par de chaves EdDSA do binário em C
│   ├── minisign.key/pub # Par de chaves para a assinatura externa
│   ├── SHA256SUMS.txt   # DNA (Hashes) das releases oficiais
│   └── *.minisig        # Assinaturas externas geradas no build
├── build/               # Output de Binários: Executáveis crus gerados pós-compilação
├── bzip/                # Output de Zips: Compactação dos binários + .minisig em .zip
├── build.sh             # Script rápido e genérico para builds locais (Devs)
├── Make-NeonX.sh        # Builder Oficial (Cross-compile via Zig + Assinaturas(Não público.)) 
├── verify.sh            # Script Bash de auditoria de integridade
├── CHANGELOG.md         # Registro arquitetural das atualizações
└── README.md            # Documentação principal

```
## 🚀 Compilando como Desenvolvedor
Para desenvolvimento rotineiro e testes locais, você não precisa cruzar a compilação ou gerar chaves. Use o script de build rápido:
```bash
bash build.sh
```
Este script detectará automaticamente o seu compilador (clang ou gcc) e criará o binário em ./build/neonx.
> **Aviso de Status:** Por padrão, esta compilação será marcada como uma build de desenvolvimento. O sistema de integridade identificará que não há uma assinatura Ed25519 anexada e classificará a build como MODIFICADA no --version. Isso é o comportamento esperado.
> 
## 🔐 A Arquitetura de "Dupla Blindagem"
O NeonX utiliza uma arquitetura de segurança de "beco sem saída" (baseada no **Monocypher**), dividida em duas camadas indissociáveis.

### 1. Blindagem Interna (Matemática em C)
A função check_integrity() lê o próprio arquivo executável (/proc/self/exe ou APIs equivalentes), extrai os últimos 128 bytes (a assinatura EdDSA) e valida contra a PUBLIC_KEY rigidamente definida em integrity.c.
Se a assinatura não for válida ou o código for modificado, a variável global de status denuncia a build. O executável **não se autodestrói**, priorizando a transparência e permitindo uso livre, mas informa o status real em --version.

### 2. Blindagem Externa e Script de Verificação
As builds de lançamento geradas pelo Make-NeonX.sh não só recebem a assinatura interna, mas também são assinadas por uma ferramenta externa (minisign) e têm seus hashes (SHA256) registrados.
O script verify.sh orquestra essas informações:
 1. Ele baixa a lista de hashes e a chave pública do servidor do GitHub em tempo real.
 2. Checa o hash do arquivo local.
 3. Checa a assinatura .minisig.
 4. **Aciona a Flag Oculta:** Invoca ./neonx --verify-sig, obrigando o binário em C a fazer a matemática do EdDSA e confessar, via exit_code, se é legítimo.

## 🛠️ Detalhes de Engenharia e Build System
### Cross-Compilation Extremamente Portátil
O builder oficial (Make-NeonX.sh) utiliza o zig cc (Zig C++ Compiler) para aplicar *Cross-Compilation* nativamente. A partir de um único ambiente (como o Termux), o script é capaz de forjar binários perfeitamente otimizados para linux-x64, linux-arm64, windows-x64, macos-arm64, entre outros.

### Unity Build nas Ferramentas (Tools)
As ferramentas em tools/ (keygen.c e sign_binary.c) são executadas no ambiente host durante a compilação. Para evitar a complexidade de configuração de *Linker* (ld) na hora de invocar a criptografia durante o fluxo do script bash, elas utilizam o conceito de **Unity Build**:
Elas fazem a inclusão direta do arquivo .c da biblioteca (#include "../src/monocypher.c"), engolindo todo o motor criptográfico em um único arquivo de compilação autossuficiente e nativo.

### Matemática de Ponto Fixo (Q16.16)
O motor de renderização (shaders.c) foi purgado do uso de pontos flutuantes (float / double) no loop de desenho principal. Toda a trigonometria utiliza tabelas de busca pré-calculadas (*Look-Up Tables*) e aritmética de inteiros (32-bits em formato Q16.16), o que garante desempenho esmagador mesmo em SoCs mobile mais antigos ou ambientes sem FPU.

## 📝 Testando suas Alterações
Após qualquer alteração nos arquivos .c, valide o sistema localmente:
 1. **Teste de Metadados e Integridade:**
   ```bash
   ./build/neonx --version
   ./build/neonx --verify-sig   # Deve retornar FAIL na compilação do build.sh
   ```
 2. **Teste do Motor de Ponto Fixo (Shaders):**
   ```bash
   cat arquivo_grande.txt | ./build/neonx --preset matrix -L
   ```
 3. **Teste de Memória:**
   Se possível, rode a execução via valgrind para garantir que o isolamento do buffer e o encerramento por SIGINT estão liberando a memória RAM adequadamente, evitando vazamentos (*Memory Leaks*).
