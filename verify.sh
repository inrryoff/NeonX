#!/usr/bin/env bash
set -e

# =====================================================
# NeonX Verifier v5.2 – Hash Match & Minisign
# =====================================================

RAW_BASE_URL="https://raw.githubusercontent.com/inrryoff/NeonX/main/keys"
PUBLIC_KEY_URL="${RAW_BASE_URL}/minisign.pub"
HASHES_URL="${RAW_BASE_URL}/SHA256SUMS.txt"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

BINARY="$1"

usage() {
    echo -e "${RED}Uso: $0 <arquivo_binario_ou_zip>${NC}"
    exit 1
}

if [ -z "$BINARY" ] || [ ! -f "$BINARY" ]; then
    usage
fi

for cmd in minisign sha256sum curl; do
    if ! command -v $cmd &> /dev/null; then
        echo -e "${RED}❌ Dependência faltando: $cmd${NC}"
        exit 1
    fi
done

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

echo -e "${YELLOW}⬇ Baixando chave pública oficial...${NC}"
if ! curl -f -sSL "$PUBLIC_KEY_URL" -o "$TMP_DIR/minisign.pub"; then
    echo -e "${RED}❌ Falha ao baixar chave pública.${NC}"
    exit 1
fi

# --------------------------------------------------
# 1. Extração (se for ZIP) ou definição do binário
# --------------------------------------------------
if [[ "$BINARY" == *.zip ]]; then
    echo -e "${YELLOW}📦 Extraindo $BINARY...${NC}"
    unzip -qo "$BINARY" -d "$TMP_DIR"
    binary_file=$(find "$TMP_DIR" -type f ! -name "*.minisig" ! -name "*.pub" | head -n1)
else
    binary_file="$BINARY"
fi

# --------------------------------------------------
# 2. Identificação baseada no Hash
# --------------------------------------------------
echo -e "${YELLOW}🔍 Calculando DNA (Hash) do arquivo...${NC}"
LOCAL_HASH=$(sha256sum "$binary_file" | awk '{print $1}')

echo -e "${YELLOW}⬇ Baixando lista de hashes oficiais...${NC}"
if ! curl -f -sSL "$HASHES_URL" -o "$TMP_DIR/SHA256SUMS.txt"; then
    echo -e "${RED}❌ Falha ao obter a lista de hashes oficiais do repositório.${NC}"
    exit 1
fi

REAL_NAME=$(grep "$LOCAL_HASH" "$TMP_DIR/SHA256SUMS.txt" | awk '{print $2}' | head -n1)

if [ -z "$REAL_NAME" ]; then
    echo ""
    echo -e "${RED}❌ ALERTA: Arquivo desconhecido ou modificado!${NC}"
    echo -e "${RED}   O hash deste arquivo ($LOCAL_HASH) não bate com NENHUMA${NC}"
    echo -e "${RED}   release oficial do NeonX.${NC}"
    echo -e "${YELLOW}   Possíveis causas:${NC}"
    echo "   1. O binário foi alterado/infectado."
    echo "   2. É uma versão antiga que não está na lista de hashes atual."
    exit 1
fi

echo -e "${GREEN}✅ Identidade confirmada: ${CYAN}$REAL_NAME${NC}"

# --------------------------------------------------
# 3. Baixar a assinatura exata e verificar
# --------------------------------------------------
expected_sig_name="${REAL_NAME}.minisig"
REMOTE_SIG_URL="${RAW_BASE_URL}/${expected_sig_name}"

echo -e "${YELLOW}⬇ Baixando assinatura específica ($expected_sig_name)...${NC}"
if ! curl -f -sSL "$REMOTE_SIG_URL" -o "$TMP_DIR/$expected_sig_name"; then
    echo -e "${RED}❌ Assinatura não encontrada no servidor para $REAL_NAME.${NC}"
    exit 1
fi

sig_file="$TMP_DIR/$expected_sig_name"

echo -e "${YELLOW}🔐 Verificando assinatura Minisign...${NC}"

set +e
output=$(minisign -V -x "$sig_file" -p "$TMP_DIR/minisign.pub" -m "$binary_file" 2>&1)
exit_code=$?
set -e

echo -e "${YELLOW}🔍 Checando assinatura EdDSA (Monocypher/C)...${NC}"
chmod +x "$binary_file"

set +e
"$binary_file" --verify-sig > /dev/null 2>&1
internal_exit_code=$?
set -e

COMPATIBILITY_WARNING=""
if [ $internal_exit_code -eq 0 ]; then
    SIG_CHECK_MSG="${GREEN}Matematicamente Autenticada (Self-Check C)${NC}"
    INTERNAL_SIG=$(tail -c 128 "$binary_file")
    if [[ "$INTERNAL_SIG" =~ ^[0-9a-fA-F]{128}$ ]]; then
        SIG_PREVIEW="${INTERNAL_SIG:0:16}...${INTERNAL_SIG: -16}"
    else
        SIG_PREVIEW="${YELLOW}Preview indisponível${NC}"
    fi
elif [ $internal_exit_code -eq 126 ]; then
    SIG_CHECK_MSG="${YELLOW}Pendente (Arquitetura Incompatível com o Host)${NC}"
    INTERNAL_SIG=$(tail -c 128 "$binary_file")
    if [[ "$INTERNAL_SIG" =~ ^[0-9a-fA-F]{128}$ ]]; then
        SIG_PREVIEW="${INTERNAL_SIG:0:16}...${INTERNAL_SIG: -16}"
    else
        SIG_PREVIEW="${YELLOW}Preview indisponível${NC}"
    fi    
    COMPATIBILITY_WARNING="\n${YELLOW}   ⚠ AVISO: O Hash e o Minisign confirmam que o binário é\n   100% OFICIAL, mas ele não pôde ser executado nesta máquina\n   para testar a assinatura interna. Teste-o no sistema de destino.${NC}"
else
    echo -e "${RED}❌ ALERTA: A assinatura EdDSA interna falhou na validação matemática!${NC}"
    echo -e "${RED}   O arquivo foi adulterado ou corrompido.${NC}"
    exit 1
fi

if [ $exit_code -eq 0 ]; then
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo -e "${GREEN}✅ BINÁRIO OFICIAL VERIFICADO!${NC}"
    echo ""
    echo -e "${CYAN}   Binário Original: $REAL_NAME${NC}"
    echo -e "${CYAN}   Ass. Externa:     $expected_sig_name (Minisign)${NC}"
    echo -e "${CYAN}   Ass. Interna:     $SIG_CHECK_MSG${NC}"
    echo -e "${CYAN}   Hex Preview:      $SIG_PREVIEW${NC}"
    echo -e "$COMPATIBILITY_WARNING"
    echo ""
    echo -e "${GREEN}   Origem confirmada: @inrryoff${NC}"
    echo -e "${GREEN}   Este é um binário OFICIAL e íntegro.${NC}"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
else
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo -e "${RED}❌ FALHA NA VERIFICAÇÃO!${NC}"
    echo ""
    echo -e "${RED}   O hash corresponde a uma release oficial, mas a${NC}"
    echo -e "${RED}   assinatura Minisign falhou.${NC}"
    echo ""
    echo -e "${YELLOW}   Detalhes do erro:${NC}"
    echo "$output"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    exit 1
fi
