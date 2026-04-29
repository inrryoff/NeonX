#!/usr/bin/env bash

# =====================================================
# NeonX Zig Build Script (Community Edition)
# Core Original: @inrryoff
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

# Fontes e Assinatura Genérica (Exatos 26 Bytes - Sem espaços)
SOURCES=("android-n.c" "linux-n.c" "windows-n.c")
SIGNATURE_STRING="BUILD_GENERICA_INFERIOR_26"
OUTPUT_DIR="build"

# =====================================================
# INJEÇÃO DE MACROS (Builds da Comunidade)
# =====================================================
CHAVE_REAL='"BUILD_GENERICA_INFERIOR_26"'
STATUS_REAL='"GENERIC_COMMUNITY_BUILD"'
# O mantenedor não deve ter espaços para não quebrar o Zig!
MANTENEDOR_REAL='"Comunidade_Open_Source"' 

# Passando as macros para o C via -D
MACRO_FLAGS="-DCHAVE_SECRETA_NEONX=$CHAVE_REAL -DBUILD_STATUS=$STATUS_REAL -DBUILD_MAINTAINER=$MANTENEDOR_REAL"

# Flags de Compilação
SECURE_FLAGS="-fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -pie -Wl,-z,relro,-z,now $MACRO_FLAGS"
PERF_FLAGS="-O3 -ffast-math"

print_header() {
    clear
    echo -e "${CYAN}${BOLD}"
    echo "╔═══════════════════════════════════════════════════════════╗"
    echo "║          NeonX - Zig Build Script (Community)             ║"
    echo "║             Android | Linux | Windows (Cross)             ║"
    echo "╚═══════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_info() { echo -e "${BLUE}ℹ${NC} $1"; }
print_success() { echo -e "${GREEN}✓${NC} $1"; }
print_warning() { echo -e "${YELLOW}⚠${NC} $1"; }
print_error() { echo -e "${RED}✗${NC} $1"; }

# Verifica a existência dos arquivos do array e do Zig
check_env() {
    print_info "Verificando ambiente e códigos-fonte..."
    
    if ! command -v zig &> /dev/null; then
        print_error "Zig não encontrado! Instale com o gerenciador de pacotes do seu sistema."
        exit 1
    fi

    local arquivos_encontrados=0
    for src in "${SOURCES[@]}"; do
        if [ -f "$src" ]; then
            print_success "Fonte pronta para uso: $src"
            arquivos_encontrados=$((arquivos_encontrados + 1))
        else
            print_warning "Ausente: $src (Builds dependentes serão ignoradas)"
        fi
    done

    if [ "$arquivos_encontrados" -eq 0 ]; then
        print_error "Nenhum arquivo C encontrado! Coloque os arquivos na mesma pasta."
        exit 1
    fi
    
    mkdir -p "$OUTPUT_DIR"
    echo
}

sign_binary() {
    local binary="$1"
    if [ ! -f "$binary" ]; then return 1; fi
    
    truncate -s -26 "$binary" 2>/dev/null || \
    dd if="$binary" of="$binary.tmp" bs=1 count=$(($(stat -c %s "$binary") - 26)) 2>/dev/null && \
    mv "$binary.tmp" "$binary" 2>/dev/null || true
    
    echo -n "$SIGNATURE_STRING" >> "$binary"
    print_success "Assinatura injetada (26-bytes): $binary"
}

# Verificador dinâmico de compilação
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
}

# 1. Build Android/Linux Genérico
build_arm64() {
    print_info "Compilando: ARM64 Genérico (Termux/Linux)..."
    compile_target "android-n.c" "$OUTPUT_DIR/neonx_arm64" "aarch64-linux-musl" $PERF_FLAGS $SECURE_FLAGS -lm
}

# 2. Build ARM 32-bits
build_arm32() {
    print_info "Compilando: ARMv7 32-bits (Hard Float)..."
    compile_target "android-n.c" "$OUTPUT_DIR/neonx_arm32" "arm-linux-musleabihf" $PERF_FLAGS $SECURE_FLAGS -lm
}

# 3. Build Linux PC x64
build_linux_x64() {
    print_info "Compilando: Linux PC (x86_64 Estático)..."
    compile_target "linux-n.c" "$OUTPUT_DIR/neonx_linux_x64" "x86_64-linux-musl" $PERF_FLAGS $SECURE_FLAGS -lm
}

# 4. Build Linux PC x86
build_linux_x86() {
    print_info "Compilando: Linux PC (x86 Estático)..."
    compile_target "linux-n.c" "$OUTPUT_DIR/neonx_linux_x86" "x86-linux-musl" $PERF_FLAGS $SECURE_FLAGS -lm
}

# 5. Build Windows PC x64
build_windows_x64() {
    print_info "Compilando: Windows PC (x86_64)..."
    compile_target "windows-n.c" "$OUTPUT_DIR/neonx_windows_x64.exe" "x86_64-windows-gnu" $PERF_FLAGS -fstack-protector-strong -D_FORTIFY_SOURCE=2 $MACRO_FLAGS
}

# 6. Build Windows PC x86
build_windows_x86() {
    print_info "Compilando: Windows PC (x86)..."
    compile_target "windows-n.c" "$OUTPUT_DIR/neonx_windows_x86.exe" "x86-windows-gnu" $PERF_FLAGS -fstack-protector-strong -D_FORTIFY_SOURCE=2 $MACRO_FLAGS
}

build_all() {
    print_info "Iniciando compilação de arsenal completo..."
    echo
    build_arm64
    build_arm32
    build_linux_x64
    build_linux_x86
    build_windows_x64
    build_windows_x86
    echo
    print_success "Processo de compilação em lote finalizado!"
}

show_menu() {
    print_header
    check_env
    echo -e "${BOLD}Targets de Compilação Zig:${NC}"
    echo -e "  ${GREEN}1${NC}) Android/Linux aarch64 (Usa: android-n.c)"
    echo -e "  ${GREEN}2${NC}) Android armeabi   (Usa: android-n.c)"
    echo -e "  ${GREEN}3${NC}) Linux PC x64       (Usa: linux-n.c)"
    echo -e "  ${GREEN}4${NC}) Linux PC x86       (Usa: linux-n.c)"
    echo -e "  ${GREEN}5${NC}) Windows PC x64     (Usa: windows-n.c)"
    echo -e "  ${GREEN}6${NC}) Windows PC x86     (Usa: windows-n.c)"
    echo -e "  ${GREEN}7${NC}) 🚀 COMPILAR TODAS AS PLATAFORMAS"
    echo -e "  ${GREEN}0${NC}) Sair"
    echo
    read -p "Opção: " choice
    
    echo
    case $choice in
        1) build_arm64 ;;
        2) build_arm32 ;;
        3) build_linux_x64 ;;
        4) build_linux_x86 ;;
        5) build_windows_x64 ;;
        6) build_windows_x86 ;;
        7) build_all ;;
        0) echo -e "${GREEN}Encerrando o ambiente de build. Até logo!${NC}"; exit 0 ;;
        *) print_error "Opção inválida."; sleep 1 ;;
    esac
}

while true; do
    show_menu
    echo
    read -p "Pressione Enter para voltar ao menu..."
done
