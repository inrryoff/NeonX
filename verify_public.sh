#!/usr/bin/env bash

GITHUB_RAW="https://raw.githubusercontent.com/inrryoff/neonx/main/verified_bins.txt"
VERIFIED_FILE="/tmp/neonx_verified_bins.txt"
BINARY="$1"
HASH_TOOL="./neonx_hash"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

if [ -z "$BINARY" ] || [ ! -f "$BINARY" ]; then
    echo -e "${RED}Uso: $0 <binario>${NC}"
    echo "Exemplo: $0 ./build/neonx_arm64"
    exit 1
fi

echo -e "${YELLOW}⬇  Baixando registro oficial do GitHub...${NC}"

if command -v curl &> /dev/null; then
    curl -sSL "$GITHUB_RAW" -o "$VERIFIED_FILE"
elif command -v wget &> /dev/null; then
    wget -q "$GITHUB_RAW" -O "$VERIFIED_FILE"
else
    echo -e "${RED}❌ curl ou wget não encontrados.${NC}"
    exit 1
fi

if [ ! -f "$VERIFIED_FILE" ] || [ ! -s "$VERIFIED_FILE" ]; then
    echo -e "${RED}❌ Falha ao baixar registro oficial.${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Registro oficial baixado.${NC}"

if [ ! -f "$HASH_TOOL" ]; then
    echo -e "${RED}❌ Calculador de hash ($HASH_TOOL) não encontrado.${NC}"
    echo "   Compile com: zig cc neonx_hash.c -o neonx_hash -target aarch64-linux-musl"
    rm -f "$VERIFIED_FILE"
    exit 1
fi

BINARY_NAME=$(basename "$BINARY")
EXPECTED_HASH=$(grep "^$BINARY_NAME " "$VERIFIED_FILE" | awk '{print $2}')

if [ -z "$EXPECTED_HASH" ]; then
    echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${RED}❌ $BINARY_NAME NÃO consta no registro oficial!${NC}"
    echo -e "${RED}   Este binário NÃO é de responsabilidade do mantenedor.${NC}"
    echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    rm -f "$VERIFIED_FILE"
    exit 1
fi

ACTUAL_HASH=$("$HASH_TOOL" "$BINARY")

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
if [ "$ACTUAL_HASH" = "$EXPECTED_HASH" ]; then
    echo -e "${GREEN}✅ $BINARY_NAME verificado com sucesso!${NC}"
    echo -e "${GREEN}   Hash oficial: $EXPECTED_HASH${NC}"
    echo -e "${GREEN}   Origem confirmada: @inrryoff${NC}"
    echo -e "${GREEN}   Este é um binário OFICIAL.${NC}"
else
    echo -e "${RED}❌ $BINARY_NAME ADULTERADO!${NC}"
    echo -e "${RED}   Hash esperado: $EXPECTED_HASH${NC}"
    echo -e "${RED}   Hash atual:    $ACTUAL_HASH${NC}"
    echo -e "${RED}   Este binário NÃO é oficial e pode estar comprometido.${NC}"
fi
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

rm -f "$VERIFIED_FILE"

if [ "$ACTUAL_HASH" = "$EXPECTED_HASH" ]; then
    exit 0
else
    exit 1
fi