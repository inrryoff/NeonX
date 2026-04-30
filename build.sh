#!/usr/bin/env bash
set -e

# Cores (opcional)
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

# Configurações oficiais (pode sobrescrever via variáveis de ambiente)
VERSION="${VERSION:-2.0.1-STABLE}"
BUILD_STATUS="${BUILD_STATUS:-OFFICIAL_STABLE_BUILD}"
BUILD_MAINTAINER="${BUILD_MAINTAINER:-@inrryoff}"
SECRET_KEY="${SECRET_KEY:-KNEONX_VERIFIED_OFICIAL_KAY_BY_INRRYOFF}"

# Diretórios
SRC_DIR="src"
TOOLS_DIR="tools"
OUTPUT_DIR="build"
VERIFIED_FILE="verified_bins.txt"

# Macros
MACRO_FLAGS='-DVERSION="'"$VERSION"'" -DSECRET_KEY="'"$SECRET_KEY"'" -DBUILD_STATUS="'"$BUILD_STATUS"'" -DBUILD_MAINTAINER="'"$BUILD_MAINTAINER"'"'
PERF_FLAGS="-O3 -ffast-math -fstack-protector-strong -D_FORTIFY_SOURCE=2"

# --------------------------------------------------
# 1. Compilar ferramentas (selar e neonx_hash)
echo -e "${CYAN}Compilando ferramentas de release...${NC}"
compile_tool() {
    local src="$TOOLS_DIR/$1"
    local out="$TOOLS_DIR/$2"
    if [ ! -f "$out" ]; then
        zig cc "$src" -o "$out" -target $(uname -m)-linux-musl -O2
    fi
}
mkdir -p "$TOOLS_DIR"
compile_tool "selar.c" "selar"
compile_tool "neonx_hash.c" "neonx_hash"

# --------------------------------------------------
# 2. Funções de assinatura e registro (mesma lógica sua)
sign_binary() {
    local binary="$1"
    local seal_hex_len=$(( ${#SECRET_KEY} * 2 ))
    # Remove possível selo antigo
    truncate -s -$seal_hex_len "$binary" 2>/dev/null || true
    # Gera novo selo
    local new_seal
    new_seal=$($TOOLS_DIR/selar "$binary" "$SECRET_KEY" | sed -n '2p')
    printf '%s' "$new_seal" >> "$binary"
    echo -e "${GREEN}✓ Selo injetado em $binary${NC}"
}

register_binary() {
    local binary="$1"
    local public_hash
    public_hash=$($TOOLS_DIR/neonx_hash "$binary")
    local name=$(basename "$binary")
    # Registra no verified_bins.txt
    echo "@inrryoff|$name|$public_hash" >> "$VERIFIED_FILE"
    echo -e "${YELLOW}ℹ Registrado: $name ($public_hash)${NC}"
}

# --------------------------------------------------
# 3. Compilação para cada plataforma
compile_release() {
    local target="$1"
    local output="$2"
    shift 2
    echo -e "${CYAN}Compilando $output...${NC}"
    mkdir -p "$OUTPUT_DIR"
    zig cc "$SRC_DIR"/*.c -o "$OUTPUT_DIR/$output" -target "$target" -static \
        $PERF_FLAGS $MACRO_FLAGS -lm
    sign_binary "$OUTPUT_DIR/$output"
    register_binary "$OUTPUT_DIR/$output"
}

# --------------------------------------------------
# 4. Menu ou lista de targets
echo "Escolha as plataformas para release oficial:"
options=("Linux x86_64" "Linux x86" "ARM64" "ARM32" "Windows x86_64" "Windows x86" "Todas")
targets=("x86_64-linux-musl" "x86-linux-musl" "aarch64-linux-musl" "arm-linux-musleabihf" "x86_64-windows-gnu" "x86-windows-gnu")
binaries=("neonx" "neonx_x86" "neonx_arm64" "neonx_arm32" "neonx.exe" "neonx_x86.exe")

select opt in "${options[@]}"; do
    case $REPLY in
        1|2|3|4|5|6)
            idx=$((REPLY-1))
            compile_release "${targets[$idx]}" "${binaries[$idx]}"
            break
            ;;
        7)
            for i in "${!targets[@]}"; do
                compile_release "${targets[$i]}" "${binaries[$i]}"
            done
            break
            ;;
        *) echo "Inválido";;
    esac
done

echo -e "${GREEN}Release oficial concluída.${NC}"