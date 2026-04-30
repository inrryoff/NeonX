#!/usr/bin/env bash

# =====================================================
# NeonX Ultimate Zig Build Script v2.0 (Unified)
# Author: Inrryoff | Security: NeonX-Hash (FNV-1a)
# =====================================================

set -e

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# Configurações de Assinatura
SIG_SIZE=8
SOURCE_FILE=( "linux-n.c" "windows-n.c" "android-n.c" )
VERIFIED_FILE="verified_bins.txt"
OUTPUT_DIR="build"
SEALER_BIN="./selar"
STATUS_REAL='"OFFICIAL_STABLE_BUILD"'
MANTENEDOR_REAL='"@inrryoff"'
KAY="KNEONX_VERIFIED_BY_INRRYOFF"
KAY_C="\"$KAY\""

# Flags de Compilação
MACRO_FLAGS="-DSECRET_KEY=$KAY_C -DBUILD_STATUS=$STATUS_REAL -DBUILD_MAINTAINER=$MANTENEDOR_REAL"
SECURE_FLAGS="-fstack-protector-strong -D_FORTIFY_SOURCE=2 $MACRO_FLAGS"
PERF_FLAGS="-O3 -ffast-math"

print_header() {
    clear
    echo -e "${CYAN}${BOLD}"
    echo "╔═══════════════════════════════════════════════════════════╗"
    echo "║                NeonX v2.0 - Universal Build               ║"
    echo "║           Security: FNV-1a Dynamic Signature              ║"
    echo "╚═══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_info() { echo -e "${BLUE}ℹ${NC} $1"; }
print_success() { echo -e "${GREEN}✓${NC} $1"; }
print_warning() { echo -e "${YELLOW}⚠${NC} $1"; }
print_error() { echo -e "${RED}✗${NC} $1"; }

init_verified_file() {
    if [ ! -f "$VERIFIED_FILE" ]; then
        {
            echo "# NeonX Verified Binaries - Build oficial"
            echo "# Mantenedor: @inrryoff"
            echo "# ATENÇÃO: Binários não listados aqui NÃO são oficiais"
            echo "# --------------------------------------------------"
            echo ""
        } > "$VERIFIED_FILE"
        print_info "Arquivo de registro criado: $VERIFIED_FILE"
    fi
}

check_env() {
    print_info "Verificando ambiente..."
    
    if ! command -v zig &> /dev/null; then
        print_error "Zig não encontrado!"
        exit 1
    fi

    for src in "${SOURCE_FILE[@]}"; do
        if [ ! -f "$src" ]; then
            print_warning "Arquivo $src não encontrado. Builds que dependem dele serão pulados."
        fi
    done

    init_verified_file
    
    compile_tool "selar.c" "./selar"
    compile_tool "neonx_hash.c" "./neonx_hash"
    
    mkdir -p "$OUTPUT_DIR"
}

generate_public_hash() {
    local binary="$1"
    if [ ! -f "$binary" ]; then return 1; fi
    
    local public_hash
    public_hash=$(./neonx_hash "$binary" 2>/dev/null)
    
    if [ -z "$public_hash" ]; then
        print_error "Falha ao calcular hash público de $binary"
        return 1
    fi
    
    echo "$public_hash"
}

compile_tool() {
    local source="$1"
    local output="$2"
    
    if [ -f "$output" ]; then
        return 0 
    fi
    
    local arch
    arch=$(uname -m)
    local target
    
    case "$arch" in
        aarch64) target="aarch64-linux-musl" ;;
        x86_64)  target="x86_64-linux-musl" ;;
        armv7l)  target="arm-linux-musleabihf" ;;
        *)
            print_error "Arquitetura $arch não suportada."
            return 1
            ;;
    esac
    
    print_warning "Compilando $output..."
    zig cc "$source" -o "$output" -target "$target"
    print_success "$output pronto."
}

register_binary() {
    local binary_name="$1"
    local public_hash="$2"
    
    if [ -z "$public_hash" ]; then
        return 1
    fi
    
    if [ -f "$VERIFIED_FILE" ]; then
        sed -i "/^$binary_name /d" "$VERIFIED_FILE"
    fi
    
    echo "$binary_name $public_hash" >> "$VERIFIED_FILE"
    print_info "Registrado: $binary_name -> $public_hash"
}

sign_binary() {
    local binary="$1"
    if [ ! -f "$binary" ]; then return 1; fi
    
    local seal_hex_len=$(( ${#KAY} * 2 )) 

    truncate -s -$seal_hex_len "$binary" 2>/dev/null || true

    local output
    output=$("$SEALER_BIN" "$binary" "$KAY") || {
        print_error "Falha ao gerar selo criptográfico."
        return 1
    }
    
    local new_seal
    new_seal=$(echo "$output" | sed -n '2p')

    printf '%s' "$new_seal" >> "$binary"
    print_success "Selo criptográfico injetado."

    local public_hash
    public_hash=$(./neonx_hash "$binary")
    
    local binary_name
    binary_name=$(basename "$binary")
    register_binary "$binary_name" "$public_hash"
}

compile_target() {
    local source_file="$1"
    local output_file="$2"
    local target_arch="$3"
    shift 3
    local extra_flags="$@"

    if [ ! -f "$source_file" ]; then
        print_error "Pulando $output_file: '$source_file' não encontrado."
        return 1
    fi

    zig cc "$source_file" -o "$output_file" -target "$target_arch" $extra_flags
    sign_binary "$output_file"
    rm -f "${output_file%.exe}.pdb"
}

build_g24() {
    print_info "Compilando: ARM64 Extreme (Cortex-A75)..."
    compile_target "android-n.c" "$OUTPUT_DIR/neonx" "aarch64-linux-musl" -mcpu=cortex_a75 $PERF_FLAGS $SECURE_FLAGS -lm
}

build_arm64() {
    print_info "Compilando: ARM64 Genérico (Termux/Linux)..."
    compile_target "android-n.c" "$OUTPUT_DIR/neonx_arm64" "aarch64-linux-musl" $PERF_FLAGS $SECURE_FLAGS -lm
}

build_arm32() {
    print_info "Compilando: ARMv7 32-bits (Hard Float)..."
    compile_target "android-n.c" "$OUTPUT_DIR/neonx_arm32" "arm-linux-musleabihf" $PERF_FLAGS $SECURE_FLAGS -lm
}

build_linux_x64() {
    print_info "Compilando: Linux PC (x86_64 Estático)..."
    compile_target "linux-n.c" "$OUTPUT_DIR/neonx_linux_x64" "x86_64-linux-musl" $PERF_FLAGS $SECURE_FLAGS -lm
}

build_linux_x86() {
    print_info "Compilando: Linux PC (x86 Estático)..."
    compile_target "linux-n.c" "$OUTPUT_DIR/neonx_linux_x86" "x86-linux-musl" $PERF_FLAGS $SECURE_FLAGS -lm
}

build_windows_x64() {
    print_info "Compilando: Windows PC (x86_64)..."
    compile_target "windows-n.c" "$OUTPUT_DIR/neonx_windows_x64.exe" "x86_64-windows-gnu" $PERF_FLAGS -fstack-protector-strong -D_FORTIFY_SOURCE=2 $MACRO_FLAGS
}

build_windows_x86() {
    print_info "Compilando: Windows PC (x86)..."
    compile_target "windows-n.c" "$OUTPUT_DIR/neonx_windows_x86.exe" "x86-windows-gnu" $PERF_FLAGS -fstack-protector-strong -D_FORTIFY_SOURCE=2 $MACRO_FLAGS
}

build_all() {
    print_info "Iniciando compilação de arsenal completo..."
    set +e
    build_g24
    build_arm64
    build_arm32
    build_linux_x64
    build_linux_x86
    build_windows_x64
    build_windows_x86
    set -e
    print_success "Processo de compilação em lote finalizado!"
}

show_menu() {
    print_header
    check_env
    echo -e "${BOLD}Targets de Compilação Zig:${NC}"
    echo -e "  ${GREEN}1${NC}) Moto G24 Pro   (Usa: android-n.c)"
    echo -e "  ${GREEN}2${NC}) Termux Padrão  (Usa: android-n.c)"
    echo -e "  ${GREEN}3${NC}) Android Antigo (Usa: android-n.c)"
    echo -e "  ${GREEN}4${NC}) Linux PC x64       (Usa: linux-n.c)"
    echo -e "  ${GREEN}5${NC}) Linux PC x86       (Usa: linux-n.c)"
    echo -e "  ${GREEN}6${NC}) Windows PC x64     (Usa: windows-n.c)"
    echo -e "  ${GREEN}7${NC}) Windows PC x86     (Usa: windows-n.c)"
    echo -e "  ${GREEN}8${NC}) 🚀 COMPILAR TODAS AS PLATAFORMAS"
    echo -e "  ${GREEN}0${NC}) Sair"
    echo
    read -p "Opção: " choice
    
    echo
    case $choice in
        1) build_g24 ;;
        2) build_arm64 ;;
        3) build_arm32 ;;
        4) build_linux_x64 ;;
        5) build_linux_x86 ;;
        6) build_windows_x64 ;;
        7) build_windows_x86 ;;
        8) build_all ;;
        0) echo -e "${GREEN}Encerrando o ambiente de build. Até logo!${NC}"; exit 0 ;;
        *) print_error "Opção inválida."; sleep 1 ;;
    esac
}

while true; do
    show_menu
    echo
    read -p "Pressione Enter para voltar ao menu..."
done
