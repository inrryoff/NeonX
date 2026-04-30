#!/usr/bin/env bash

# =====================================================
# NeonX Public Verifier v4.0
# Verifica pela hash, não pelo nome do arquivo
# =====================================================

GITHUB_RAW="https://raw.githubusercontent.com/inrryoff/NeonX/main/verified_bins.txt"
GITHUB_RELEASES="https://raw.githubusercontent.com/inrryoff/NeonX/main/bin"
HASH_TOOL="./neonx_hash"
BINARY="$1"

if [ -d "$TMPDIR" ] && [ -w "$TMPDIR" ]; then
    TMP_DIR="$TMPDIR"
elif [ -d "$HOME/tmp" ] && [ -w "$HOME/tmp" ]; then
    TMP_DIR="$HOME/tmp"
else
    TMP_DIR="/tmp"
fi
VERIFIED_FILE="${TMP_DIR}/neonx_verified_$$_$RANDOM.txt"
rm -f "$VERIFIED_FILE" 2>/dev/null

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

if [ -z "$BINARY" ] || [ ! -f "$BINARY" ]; then
    echo -e "${RED}Uso: $0 <arquivo_binario>${NC}"
    echo ""
    echo -e "${CYAN}Exemplos:${NC}"
    echo "  $0 neonx"
    echo "  $0 ./meu_neonx_arm64"
    echo "  $0 /caminho/qualquer/neonx.exe"
    exit 1
fi

echo -e "${YELLOW}⬇  Baixando registro oficial...${NC}"
rm -f "$VERIFIED_FILE"

if command -v curl &> /dev/null; then
    curl -sSL "$GITHUB_RAW" -o "$VERIFIED_FILE" || {
        echo -e "${RED}❌ Falha ao baixar registro.${NC}"
        exit 1
    }
elif command -v wget &> /dev/null; then
    wget -q "$GITHUB_RAW" -O "$VERIFIED_FILE" || {
        echo -e "${RED}❌ Falha ao baixar registro.${NC}"
        exit 1
    }
else
    echo -e "${RED}❌ curl ou wget não encontrados.${NC}"
    exit 1
fi

if [ ! -s "$VERIFIED_FILE" ]; then
    echo -e "${RED}❌ Registro oficial vazio ou não disponível.${NC}"
    exit 1
fi

# =============================================
# Download do motor de hash (se necessário)
# =============================================
if [ ! -f "$HASH_TOOL" ]; then
    echo -e "${YELLOW}🔍 Baixando motor de hash...${NC}"
    
    OS="$(uname -s 2>/dev/null || echo 'Windows')"
    ARCH="$(uname -m 2>/dev/null || echo 'x86_64')"
    
    if [ "$OS" = "Windows" ] || [ "$OS" = "MINGW64_NT" ] || [ "$OS" = "MSYS_NT" ]; then
        if [ "$PROCESSOR_ARCHITECTURE" = "AMD64" ]; then ARCH="x86_64"
        elif [ "$PROCESSOR_ARCHITECTURE" = "x86" ]; then ARCH="i686"; fi
        HASH_TOOL="./neonx_hash.exe"
    fi
    
    case "$ARCH" in
        aarch64|arm64)  TOOL_SRC="neonx_hash_arm64" ;;
        x86_64|amd64)   TOOL_SRC="neonx_hash_x86_64" ;;
        armv7l|armhf)   TOOL_SRC="neonx_hash_arm32" ;;
        i686|i386)      TOOL_SRC="neonx_hash_x86" ;;
        *) echo -e "${RED}❌ Arquitetura não suportada.${NC}"; rm -f "$VERIFIED_FILE"; exit 1 ;;
    esac
    
    [ "$OS" = "Windows" ] || [ "$OS" = "MINGW64_NT" ] || [ "$OS" = "MSYS_NT" ] && TOOL_SRC="${TOOL_SRC}.exe"
    
    if command -v curl &> /dev/null; then
        curl -sSL "${GITHUB_RELEASES}/${TOOL_SRC}" -o "$HASH_TOOL"
    else
        wget -q "${GITHUB_RELEASES}/${TOOL_SRC}" -O "$HASH_TOOL"
    fi
    chmod +x "$HASH_TOOL" 2>/dev/null || true
    echo -e "${GREEN}✅ Motor de hash pronto.${NC}"
fi

# =============================================
# Calcula o hash do binário fornecido
# =============================================
echo -e "${YELLOW}🔐 Calculando hash do arquivo...${NC}"
ACTUAL_HASH=$("$HASH_TOOL" "$BINARY")

if [ -z "$ACTUAL_HASH" ]; then
    echo -e "${RED}❌ Falha ao calcular hash.${NC}"
    rm -f "$VERIFIED_FILE"
    exit 1
fi

# =============================================
# Procura a hash no registro oficial
# =============================================
FOUND_LINE=$(grep "|$ACTUAL_HASH$" "$VERIFIED_FILE")
AUTHOR=$(echo "$FOUND_LINE" | awk -F'|' '{print $1}')
BINARY_NAME=$(echo "$FOUND_LINE" | awk -F'|' '{print $2}')

if [ -z "$FOUND_LINE" ]; then
    echo ""
    echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${RED}❌ Binário NÃO É OFICIAL!${NC}"
    echo -e "${RED}   Hash: $ACTUAL_HASH${NC}"
    echo -e "${RED}   Esta hash não consta no registro oficial.${NC}"
    echo -e "${RED}   O arquivo pode estar corrompido ou não ser legítimo.${NC}"
    echo -e "${RED}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    rm -f "$VERIFIED_FILE"
    exit 1
fi

# Extrai informações da linha encontrada
BINARY_NAME=$(echo "$FOUND_LINE" | awk -F'|' '{print $1}')
PLATFORM=$(echo "$FOUND_LINE" | awk -F'|' '{print $2}')

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo -e "${GREEN}✅ BINÁRIO OFICIAL VERIFICADO!${NC}"
echo ""
echo -e "${CYAN}   Nome oficial: ${BINARY_NAME}${NC}"
echo -e "${CYAN}   Plataforma:   ${PLATFORM}${NC}"
echo -e "${CYAN}   Hash:         ${ACTUAL_HASH}${NC}"
echo ""
echo -e "${GREEN}   Origem confirmada: @inrryoff${NC}"
echo -e "${GREEN}   Este é um binário OFICIAL e íntegro.${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

rm -f "$VERIFIED_FILE"
exit 0