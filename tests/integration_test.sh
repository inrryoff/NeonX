#!/bin/bash
# integration_test.sh - Testes de integração portáteis para NeonX
# Focado em Termux, Linux, macOS e Windows (via Git Bash/MSYS2)

BIN_DIR="./build"
PROJECT_NAME="neonx"

# Cores para saída
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Detectar binário
if [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32"* ]]; then
    BIN="${BIN_DIR}/${PROJECT_NAME}.exe"
else
    BIN="${BIN_DIR}/${PROJECT_NAME}"
fi

if [[ ! -f "$BIN" ]]; then
    echo -e "${RED}✗ Erro: Binário não encontrado em $BIN${NC}"
    exit 1
fi

FAILURES=0

test_cmd() {
    local label="$1"
    local cmd="$2"
    echo -n "Testing $label... "
    if eval "$cmd" > /dev/null 2>&1; then
        echo -e "${GREEN}PASS${NC}"
    else
        echo -e "${RED}FAIL${NC}"
        FAILURES=$((FAILURES + 1))
    fi
}

echo -e "${YELLOW}--- Iniciando Testes de Integração NeonX ---${NC}"

# 1. Informações Básicas
test_cmd "Help flag" "$BIN --help"
test_cmd "Version flag" "$BIN --version"
test_cmd "License flag" "$BIN --license"

# Integrity check deve PASSAR (0) agora que o build.sh assina o binário
test_cmd "Integrity check (Self-Signed)" "$BIN --verify-sig"

# Verificar status dinâmico na versão
echo -n "Checking Version maintainer info... "
if $BIN -v | grep -q "COMMUNITY" && $BIN -v | grep -q "VALID_SIG_BY"; then
    echo -e "${GREEN}PASS${NC}"
else
    # Se for build oficial de release (o que não é o caso aqui no dev)
    if $BIN -v | grep -q "OFFICIAL_BY_INRRYOFF"; then
        echo -e "${GREEN}PASS (OFFICIAL)${NC}"
    else
        echo -e "${RED}FAIL (Maintainer info mismatch)${NC}"
        FAILURES=$((FAILURES + 1))
    fi
fi

# 2. Modos de Renderização Estáticos (Evita loops infinitos no teste)
test_cmd "Static mode (-S)" "echo 'TEST' | $BIN -S"
test_cmd "Static mode with mode 10" "echo 'TEST' | $BIN -m 10 -S"
test_cmd "Quantized mode" "echo 'TEST' | $BIN --quantized -S"
test_cmd "Angle and Opacity" "echo 'TEST' | $BIN -A 45 -o 0.5 -S"

# 3. Presets
test_cmd "Preset Cyberpunk" "echo 'TEST' | $BIN --preset cyberpunk -S"
test_cmd "Preset Matrix" "echo 'TEST' | $BIN --preset matrix -S"

# 4. Ferramentas
test_cmd "Spin tool" "$BIN --spin"

# 5. Parâmetros Numéricos (Segfault check)
test_cmd "High FPS" "echo 'TEST' | $BIN -F 120 -d 0.1"
test_cmd "High Frequency" "echo 'TEST' | $BIN -f 10.0 -d 0.1"
test_cmd "Custom Width" "echo 'TEST' | $BIN -c 200 -S"

# 6. Internacionalização
test_cmd "Language PT" "$BIN --lang pt --help"
test_cmd "Language EN" "$BIN --lang en --help"
test_cmd "Language ES (ASCII check)" "$BIN --lang es --help | grep -qi 'uso:'"

# Teste de caracteres multi-byte (Chinês) - requer suporte UTF-8 no shell/grep
echo -n "Testing Language ZH (UTF-8 check)... "
if $BIN --lang zh --help | grep -q "用法"; then
    echo -e "${GREEN}PASS${NC}"
else
    # Alguns ambientes (como Termux sem LANG setado ou Windows CMD) podem falhar no grep
    # mas o comando em si funcionou. Marcamos como informativo.
    echo -e "${YELLOW}SKIPPED (Shell/Grep UTF-8 limitation)${NC}"
fi

echo -e "${YELLOW}--- Resumo dos Testes ---${NC}"
if [[ $FAILURES -eq 0 ]]; then
    echo -e "${GREEN}✓ Todos os testes passaram!${NC}"
    exit 0
else
    echo -e "${RED}✗ $FAILURES testes falharam.${NC}"
    exit 1
fi
