# 🛠️ Guia do Desenvolvedor — NeonX

Bem-vindo(a)! Este documento explica como o NeonX é estruturado, como você pode modificá-lo e como ter sua contribuição reconhecida oficialmente.

---

## 📁 Estrutura do projeto

```
NeonX/
├── src/                  # Código-fonte modular
│   ├── main.c            # Ponto de entrada, parsing de argumentos
│   ├── integrity.c/.h    # Verificação de integridade (selo + cache allow_mod)
│   ├── shaders.c/.h      # Todos os shaders e funções de cor
│   └── terminal.c/.h     # Configuração ANSI, SIGINT, exibição de versão/ajuda
├── build.sh              # Build para devs (gera hash público para submissão)
├── release.sh            # Build oficial "com selo, para @inrryoff" (não disponível no branch)
├── verified_bins.txt     # Registro de binários oficiais (hashes)
├── verified_mods.txt     # Modificações aprovadas pela comunidade (adicionado quando solicitado)
├── .github/workflows/    # CI/CD (GitHub Actions)
├── LICENSE       #Lcensa de uso
└── README.md

```

---

## 🚀 Compilando como desenvolvedor

Use o script **`build-devs.sh`** para compilar seu próprio binário modificado e obter o hash público:

```bash
bash build-devs.sh
```

Escolha a plataforma desejada. O script compilará `src/*.c` com as macros adequadas para uma build **não oficial** (aviso de integridade presente) e exibirá o hash FNV-1a do binário.

Você pode personalizar os metadados exibidos em `--version` definindo variáveis de ambiente antes de executar:

```bash
export VERSION="2.0.1-MeuMod"
export BUILD_MAINTAINER="SeuNome"
export BUILD_STATUS="MOD_EXPERIMENTAL"
bash build-devs.sh
```

> **Nota:** O build local sempre exibirá o aviso de integridade (a menos que você use `--allow-mod`). Isso é intencional – apenas o mantenedor oficial pode gerar binários sem aviso.

---

## 🧪 Testando localmente

Após compilar, execute os mesmos testes que o CI roda:

```bash
./build/neonx -v | grep -q "Status: MODIFICADO"   # deve mostrar status modificado
echo "test" | ./build/neonx --preset cyberpunk -S > /dev/null
./build/neonx -h | grep -q "Uso: cat arquivo"
```

O cache do `--allow-mod` é armazenado em `~/.neonx_cache/allow_mod`. Para limpá-lo durante testes:
```bash
rm -rf ~/.neonx_cache
```

---

## 🔐 Entendendo o sistema de integridade

- **`check_integrity()`** (em `integrity.c`) compara o selo criptográfico no final do binário com uma assinatura derivada da chave secreta (`SECRET_KEY`) e do hash FNV-1a do próprio arquivo.
- Builds oficiais têm o selo injetado pelo `release.sh` usando a ferramenta `tools/selar`.
- Builds não oficiais não possuem selo válido → `check_integrity()` retorna 2.
- O aviso amarelo é controlado pelo `main.c` e pode ser suprimido com `--allow-mod` (com cache).
- `terminal.c` usa `g_integrity_status` para exibir metadados genéricos em builds não íntegras (`Status: MODIFICADO`).

**Isso significa que ninguém consegue falsificar uma build oficial**, pois o selo depende de uma chave que só o mantenedor possui. Mas qualquer pessoa pode compilar, modificar e usar o código – o aviso apenas informa a origem.

---

## 📨 Submetendo sua modificação para aprovação

Se você criou um mod que gostaria que fosse reconhecido oficialmente:

1. Faça um fork do repositório e aplique suas alterações em `src/`.
2. Compile usando `build-devs.sh` para obter o **hash público** do binário (exibido no final).
3. Abra uma **Issue** ou **Pull Request** informando:
   - Nome do mantenedor (como deseja ser creditado)
   - Descrição da modificação (ex: "Adiciona shader aurora")
   - Hash público gerado
4. Após revisão, @inrryoff adicionará sua entrada ao arquivo `verified_mods.txt`.
5. Seu mod então aparecerá como **"Mod aprovado"** no verificador público e em futuras integrações.

---

## 🤖 CI/CD

Utilizamos GitHub Actions:

- **`build.yml`**: compila o NeonX para Linux x86_64, ARM64, ARM32, Windows x64 e x86 a cada push no branch `main` ou `dev`.
- **`tests.yml`**: compila nativamente e realiza testes funcionais (help, version, integridade, shaders, license).

Ambos os workflows usam o script `build.sh` com argumentos `--target` e `--output` para modo não interativo.

---

## 📜 Licença

Lembre-se de que qualquer modificação deve respeitar a licença original:
- Atribuição de créditos ao autor original **@inrryoff**.
- Proibida a venda do software ou de trabalhos derivados.
- O código derivado deve permanecer aberto.

Veja o texto completo com `./neonx --license`.

---

**Dúvidas? Sugestões?**  
Abra uma issue ou entre em contato com @inrryoff. Toda contribuição é bem-vinda! 🤝
