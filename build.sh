#!/usr/bin/env bash

# NeonX Generic Developer Build Script
# Use este script para compilação local rápida durante o desenvolvimento.

# Cores para o terminal
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Configurações de Build
BINARY_NAME="neonx"
SRC_DIR="src"
BUILD_DIR="build"
VERSION="2.0.1-MOD"
MAINTAINER=$(whoami)

echo -e "${CYAN}--- Iniciando Build de Desenvolvimento (NeonX v$VERSION) ---${NC}"

# Criar diretório de build
mkdir -p "$BUILD_DIR"

# Detectar compilador
if command -v clang &> /dev/null; then
    CC="clang"
elif command -v gcc &> /dev/null; then
    CC="gcc"
else
    CC="cc"
fi

echo -e "${YELLOW}Usando compilador: $CC${NC}"

# Compilação modular
# Inclui todos os arquivos .c do diretório src
$CC "$SRC_DIR"/*.c \
    -o "$BUILD_DIR/$BINARY_NAME" \
    -O2 -Wall -Wextra \
    -lm \
    -DVERSION="\"$VERSION\"" \
    -DBUILD_STATUS="\"DEVELOPMENT_BUILD\"" \
    -DBUILD_MAINTAINER="\"$MAINTAINER\""

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Compilação concluída com sucesso!${NC}"
    echo -e "O binário está em: ${CYAN}$BUILD_DIR/$BINARY_NAME${NC}"
    echo -e "Para testar, use: ${YELLOW}echo \"Hello NeonX\" | ./$BUILD_DIR/$BINARY_NAME${NC}"
else
    echo -e "\033[0;31m✗ Falha na compilação.${NC}"
    exit 1
fi
