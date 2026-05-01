#!/usr/bin/env bash
set -e

# =====================================================
# NeonX Public Verifier v5.0 – Minisign
# =====================================================
# Verifica a autenticidade e integridade de qualquer
# binário NeonX (oficial ou modificado) usando a
# assinatura Minisign e a chave pública oficial.
# =====================================================

# URLs oficiais
PUBLIC_KEY_URL="https://raw.githubusercontent.com/inrryoff/NeonX/main/keys/neonx_public.pub"
RELEASES_BASE="https://github.com/inrryoff/NeonX/releases/latest/download"

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

BINARY="$1"

usage() {
    echo -e "${RED}Uso: $0 <arquivo_binario_ou_zip> [assinatura.minisig]${NC}"
    echo ""
    echo -e "${CYAN}Exemplos:${NC}"
    echo "  $0 neonx"
    echo "  $0 ./meu_neonx_arm64"
    echo "  $0 neonx neonx_linux_x64.minisig"
    echo "  $0 neonx-linux-arm64.zip"
    exit 1
}

# Verifica argumento
if [ -z "$BINARY" ] || [ ! -f "$BINARY" ]; then
    usage
fi

# Verifica se minisign está disponível
if ! command -v minisign &> /dev/null; then
    echo -e "${RED}❌ minisign não encontrado.${NC}"
    echo -e "${YELLOW}Instale com:${NC}"
    echo "  Termux:   pkg install minisign"
    echo "  Debian/Ubuntu: sudo apt install minisign"
    echo "  macOS:    brew install minisign"
    exit 1
fi

# Cria diretório temporário
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

# --------------------------------------------------
# 1. Baixar chave pública oficial
# --------------------------------------------------
echo -e "${YELLOW}⬇  Baixando chave pública oficial...${NC}"
if ! curl -sSL "$PUBLIC_KEY_URL" -o "$TMP_DIR/neonx_public.pub"; then
    echo -e "${RED}❌ Falha ao baixar chave pública.${NC}"
    exit 1
fi
echo -e "${GREEN}✅ Chave pública obtida.${NC}"

# --------------------------------------------------
# 2. Extrair binário e assinatura (se zip)
# --------------------------------------------------
SIG_FILE_ARG="$2"

if [[ "$BINARY" == *.zip ]]; then
    echo -e "${YELLOW}📦 Extraindo $BINARY...${NC}"
    unzip -qo "$BINARY" -d "$TMP_DIR"
    sig_file=$(ls "$TMP_DIR"/*.minisig 2>/dev/null | head -n1)
    binary_file=$(find "$TMP_DIR" -type f ! -name "*.minisig" ! -name "*.pub" | head -n1)
    if [ -z "$binary_file" ]; then
        echo -e "${RED}❌ Nenhum binário encontrado no zip.${NC}"
        exit 1
    fi
else
    binary_file="$BINARY"
    # Prioridade: argumento explícito > arquivo .minisig ao lado com mesmo nome > download da release
    if [ -n "$SIG_FILE_ARG" ] && [ -f "$SIG_FILE_ARG" ]; then
        sig_file="$SIG_FILE_ARG"
        echo -e "${CYAN}ℹ Usando assinatura fornecida: $sig_file${NC}"
    elif [ -f "${binary_file}.minisig" ]; then
        sig_file="${binary_file}.minisig"
    else
        base_name=$(basename "$BINARY")
        remote_sig="${RELEASES_BASE}/${base_name}.minisig"
        echo -e "${YELLOW}⬇  Tentando baixar assinatura oficial (${base_name}.minisig)...${NC}"
        if ! curl -sSL "$remote_sig" -o "$TMP_DIR/${base_name}.minisig"; then
            echo -e "${RED}❌ Assinatura não encontrada.${NC}"
            echo -e "${YELLOW}   Use: $0 $BINARY <arquivo.minisig>${NC}"
            exit 1
        fi
        sig_file="$TMP_DIR/${base_name}.minisig"
    fi
fi

echo -e "${YELLOW}🔐 Verificando assinatura Minisign...${NC}"

# --------------------------------------------------
# 3. Executar minisign -V
# --------------------------------------------------
set +e
output=$(minisign -V -x "$sig_file" -p "$TMP_DIR/neonx_public.pub" -m "$binary_file" 2>&1)
exit_code=$?
set -e

if [ $exit_code -eq 0 ]; then
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo -e "${GREEN}✅ BINÁRIO OFICIAL VERIFICADO!${NC}"
    echo ""
    echo -e "${CYAN}   Binário:       $(basename "$binary_file")${NC}"
    echo -e "${CYAN}   Assinatura:    $(basename "$sig_file")${NC}"
    echo ""
    echo -e "${GREEN}   Origem confirmada: @inrryoff${NC}"
    echo -e "${GREEN}   Este é um binário OFICIAL e íntegro.${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
else
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo -e "${RED}❌ Binário NÃO É OFICIAL!${NC}"
    echo ""
    echo -e "${RED}   A assinatura Minisign não confere.${NC}"
    echo -e "${RED}   O arquivo pode estar corrompido, modificado${NC}"
    echo -e "${RED}   ou não ser uma build oficial do NeonX.${NC}"
    echo ""
    echo -e "${YELLOW}   Detalhes:${NC}"
    echo "$output"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    exit 1
fi