#!/usr/bin/env bash
set -e

# =====================================================
# NeonX Unified Builder v3.3 – CI/CD & Dev Edition
# =====================================================

# Cores
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

# --------------------------------------------------
# Configurações de Identidade (Sobrescrevíveis via ENV)
# --------------------------------------------------
VERSION="${VERSION:-2.0.1-MOD}"
BUILD_STATUS="${BUILD_STATUS:-DEV_BUILD}"
BUILD_MAINTAINER="${BUILD_MAINTAINER:-$(whoami)}"
DEV_FLAG="-DDEV_BUILD"

# Diretórios
SRC_DIR="src"
TOOLS_DIR="tools"
OUTPUT_DIR="build"
PRIVATE_KEY="keys/priv.key"
PUBLIC_KEY="keys/pub.key"

# Flags de Performance
PERF_FLAGS="-O3 -ffast-math -fstack-protector-strong -D_FORTIFY_SOURCE=2"

# --------------------------------------------------
# Funções Auxiliares
# --------------------------------------------------
print_info()    { echo -e "${CYAN}ℹ $1${NC}"; }
print_success() { echo -e "${GREEN}✓ $1${NC}"; }
print_warn()    { echo -e "${YELLOW}⚠ $1${NC}"; }
print_error()   { echo -e "${RED}✗ $1${NC}"; }

check_deps() {
    local deps=("gcc" "zig" "zip" "hexdump")
    for tool in "${deps[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            print_warn "Ferramenta '$tool' não encontrada. O build pode falhar."
        fi
    done
}

# --------------------------------------------------
# Gestão de Identidade e Assinatura
# --------------------------------------------------
prepare_dev_key() {
    if [ -f "$PUBLIC_KEY" ]; then
        print_info "Injetando chave pública local: $PUBLIC_KEY"
        # Converte a chave pública binária para array C (0xAA, 0xBB...)
        local bytes
        bytes=$(hexdump -v -e '1/1 "0x%02X, "' "$PUBLIC_KEY" | sed 's/, $//')
        DEV_KEY_MACRO="-DCUSTOM_PUB_KEY=$bytes"
    else
        DEV_KEY_MACRO=""
        print_warn "Chave pública não encontrada. Usando padrão do repositório."
    fi
}

sign_binary() {
    local binary="$1"
    if [ -f "$PRIVATE_KEY" ] && [ -f "$TOOLS_DIR/sign_binary" ]; then
        print_info "Assinando binário com Monocypher..."
        local sig
        sig=$("$TOOLS_DIR/sign_binary" "$binary" "$PRIVATE_KEY")
        echo -n "$sig" >> "$binary"
        print_success "Assinatura interna anexada."
    else
        print_warn "Assinatura interna pulada (Chave ou ferramenta ausente)."
    fi
}

# --------------------------------------------------
# Núcleo de Compilação
# --------------------------------------------------
compile_tool() {
    local target="$1"
    local bin_out="$2"
    local label="$3"
    local is_native="$4"

    mkdir -p "$OUTPUT_DIR"
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build: $label"

    # Preparar macros
    local macro_flags="-DVERSION=\"$VERSION\" -DBUILD_STATUS=\"$BUILD_STATUS\" -DBUILD_MAINTAINER=\"$BUILD_MAINTAINER\" $DEV_FLAG"
    prepare_dev_key

    # Pasta temporária para o ZIP
    local temp_folder="neonx_${label}"
    local temp_dir="$OUTPUT_DIR/$temp_folder"
    mkdir -p "$temp_dir"
    
    local final_bin="$temp_dir/$bin_out"

    # Compilação
    if [ "$is_native" == "true" ]; then
        gcc "$SRC_DIR"/*.c -o "$final_bin" -march=native $PERF_FLAGS $macro_flags $DEV_KEY_MACRO -lm
    else
        zig cc "$SRC_DIR"/*.c -o "$final_bin" -target "$target" -static $PERF_FLAGS $macro_flags $DEV_KEY_MACRO -lm
    fi

    # Assinatura
    sign_binary "$final_bin"

    # ZIP (Apenas se não for CI ou se solicitado)
    if command -v zip &> /dev/null; then
        (cd "$OUTPUT_DIR" && zip -q -r "neonx_${label}.zip" "$temp_folder")
        print_success "Pacote gerado: $OUTPUT_DIR/neonx_${label}.zip"
    fi

    # Se for nativo, deixa uma cópia na raiz do build
    [ "$is_native" == "true" ] && cp "$final_bin" "$OUTPUT_DIR/neonx_native"

    rm -rf "$temp_dir"
}

# --------------------------------------------------
# Processamento de Argumentos (Modo Bot/CI)
# --------------------------------------------------
AUTO_TARGET=""
AUTO_OUTPUT=""
AUTO_LABEL=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --target) AUTO_TARGET="$2"; shift 2 ;;
        --output) AUTO_OUTPUT="$2"; shift 2 ;;
        --label)  AUTO_LABEL="$2";  shift 2 ;;
        --native) AUTO_TARGET="native"; AUTO_OUTPUT="neonx"; AUTO_LABEL="native"; shift ;;
        *) shift ;;
    esac
done

check_deps

# Se o GitHub Actions passar os parâmetros, roda sem menu
if [ -n "$AUTO_TARGET" ]; then
    is_nat="false"
    [ "$AUTO_TARGET" == "native" ] && is_nat="true"
    compile_tool "$AUTO_TARGET" "$AUTO_OUTPUT" "${AUTO_LABEL:-target}" "$is_nat"
    exit 0
fi

# --------------------------------------------------
# Menu Interativo (Modo Humano/Termux)
# --------------------------------------------------
show_menu() {
    clear
    echo -e "${CYAN}╔════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║${NC} ${YELLOW}NeonX Unified Builder v3.3${NC}                         ${CYAN}║${NC}"
    echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
    echo -e "${CYAN}║${NC} 1. Linux x64 (Musl)      5. Windows x64            ${CYAN}║${NC}"
    echo -e "${CYAN}║${NC} 2. Linux x86 (Musl)      6. Windows x86            ${CYAN}║${NC}"
    echo -e "${CYAN}║${NC} 3. ARM64 (Android/Linux)  7. Todas as Plataformas   ${CYAN}║${NC}"
    echo -e "${CYAN}║${NC} 4. ARM32 (Legacy)         8. Build Nativa (Local)   ${CYAN}║${NC}"
    echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
    echo -e "${CYAN}║${NC} 0. Sair                                            ${CYAN}║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════════════════╝${NC}"
}

targets=("x86_64-linux-musl" "x86-linux-musl" "aarch64-linux-musl" "arm-linux-musleabihf" "x86_64-windows-gnu" "x86-windows-gnu")
labels=("linux-x64" "linux-x86" "linux-arm64" "linux-arm32" "windows-x64" "windows-x86")

while true; do
    show_menu
    read -p "Opção: " opt
    case $opt in
        [1-6])
            idx=$((opt-1))
            compile_tool "${targets[$idx]}" "neonx" "${labels[$idx]}" "false"
            break ;;
        7)
            for i in "${!targets[@]}"; do compile_tool "${targets[$i]}" "neonx" "${labels[$i]}" "false"; done
            break ;;
        8)
            compile_tool "native" "neonx" "native" "true"
            break ;;
        0) exit 0 ;;
        *) print_error "Opção inválida!"; sleep 1 ;;
    esac
done

print_success "Build concluída em $OUTPUT_DIR/"
