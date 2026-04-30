#!/usr/bin/env bash
set -e

# ===================================================================
# NeonX Build Script para Desenvolvedores (v2.0.1-dev)
# ===================================================================
# Este script compila o NeonX a partir de src/*.c usando Zig + musl,
# gera o hash público do binário (via neonx_hash) e orienta o
# desenvolvedor a enviar o hash para que sua modificação possa ser
# listada no arquivo verified_mods.txt do repositório oficial.
# ===================================================================

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# ------------------------------------------------------------
# Configurações que o desenvolvedor PODE alterar:
# (pode sobrescrever com export VARIÁVEL=valor antes de rodar)
VERSION="${VERSION:-2.0.1-DEV}"
BUILD_MAINTAINER="${BUILD_MAINTAINER:-Desenvolvedor Anônimo}"
BUILD_STATUS="${BUILD_STATUS:-UNOFFICIAL_MOD}"
SECRET_KEY="${SECRET_KEY:-CHAVE_MOD_LOCAL}"   # chave local, sem impacto
# ------------------------------------------------------------

SRC_DIR="src"
TOOLS_DIR="tools"
OUTPUT_DIR="build"
HASH_BIN="$TOOLS_DIR/neonx_hash"

# Macros que serão embutidas no binário
MACRO_FLAGS="-DVERSION=\"$VERSION\" -DBUILD_STATUS=\"$BUILD_STATUS\" -DBUILD_MAINTAINER=\"$BUILD_MAINTAINER\" -DSECRET_KEY=\"$SECRET_KEY\""
BASE_FLAGS="-O2 -ffast-math -fstack-protector-strong -D_FORTIFY_SOURCE=2 $MACRO_FLAGS"

print_info()  { echo -e "${BLUE}ℹ${NC} $1"; }
print_success() { echo -e "${GREEN}✓${NC} $1"; }
print_error() { echo -e "${RED}✗${NC} $1"; }
print_warn()  { echo -e "${YELLOW}⚠${NC} $1"; }

check_zig() {
    if ! command -v zig &> /dev/null; then
        print_error "Zig não encontrado. Instale com 'pkg install zig' ou https://ziglang.org/download/"
        exit 1
    fi
}

compile_neonx_hash() {
    if [ -f "$HASH_BIN" ]; then
        return 0
    fi
    if [ ! -f "$TOOLS_DIR/neonx_hash.c" ]; then
        print_warn "Ferramenta neonx_hash não encontrada em $TOOLS_DIR/."
        print_warn "O hash não será gerado automaticamente. Você pode calcular manualmente com sha256sum."
        return 1
    fi
    print_info "Compilando neonx_hash..."
    zig cc "$TOOLS_DIR/neonx_hash.c" -o "$HASH_BIN" -target $(uname -m)-linux-musl -O2
    print_success "neonx_hash compilado."
}

compile_target() {
    local target="$1"
    local output="$2"
    shift 2

    mkdir -p "$OUTPUT_DIR"
    print_info "Compilando para $target → $output ..."
    zig cc "$SRC_DIR"/*.c -o "$OUTPUT_DIR/$output" -target "$target" -static \
        $BASE_FLAGS $MACRO_FLAGS -lm "$@"

    print_success "Binário criado: $OUTPUT_DIR/$output"

    # Tenta gerar o hash público
    if [ -x "$HASH_BIN" ]; then
        local hash
        hash=$("$HASH_BIN" "$OUTPUT_DIR/$output" 2>/dev/null)
        if [ -n "$hash" ]; then
            echo ""
            echo -e "${GREEN}${BOLD}Hash público do binário:${NC}"
            echo "$hash"
            echo ""
            print_info "Envie este hash para @inrryoff junto com seu nome e descrição da modificação."
            print_info "Após aprovação, ele será adicionado em verified_mods.txt do repositório oficial."
        else
            print_warn "Falha ao calcular hash. Você pode usar sha256sum $OUTPUT_DIR/$output."
        fi
    else
        print_info "Para obter o hash: sha256sum $OUTPUT_DIR/$output"
        print_info "Envie o hash para @inrryoff para inclusão em verified_mods.txt."
    fi
}

# --------------------------------------------------
# Menu de build
show_menu() {
    echo -e "${CYAN}${BOLD}"
    echo "╔═══════════════════════════════════════════════╗"
    echo "║   NeonX Build System (Developer Edition)      ║"
    echo "╚═══════════════════════════════════════════════╝"
    echo -e "${NC}"
    echo -e "${BOLD}Metadados atuais:${NC}"
    echo -e "  Versão:       ${VERSION}"
    echo -e "  Mantenedor:   ${BUILD_MAINTAINER}"
    echo -e "  Status:       ${BUILD_STATUS}"
    echo ""
    echo "Selecione a plataforma:"
    echo "1) Linux x86_64"
    echo "2) Linux x86"
    echo "3) ARM64 (Linux/Android)"
    echo "4) ARM32 (Linux/Android)"
    echo "5) Windows x86_64"
    echo "6) Windows x86"
    echo "7) Compilar para esta máquina (auto-detecção)"
    echo "0) Sair"
    read -p "Opção: " choice
    echo

    case $choice in
        1) compile_target "x86_64-linux-musl"      "neonx" ;;
        2) compile_target "x86-linux-musl"         "neonx" ;;
        3) compile_target "aarch64-linux-musl"    "neonx" ;;
        4) compile_target "arm-linux-musleabihf"  "neonx" ;;
        5) compile_target "x86_64-windows-gnu"     "neonx.exe" ;;
        6) compile_target "x86-windows-gnu"       "neonx.exe" ;;
        7) 
            arch=$(uname -m)
            case "$arch" in
                x86_64)  compile_target "x86_64-linux-musl"     "neonx" ;;
                aarch64) compile_target "aarch64-linux-musl"   "neonx" ;;
                armv7l)  compile_target "arm-linux-musleabihf" "neonx" ;;
                i686|i386) compile_target "x86-linux-musl"     "neonx" ;;
                *)        print_error "Arquitetura desconhecida: $arch"; exit 1 ;;
            esac
            ;;
        0) exit 0 ;;
        *) print_error "Inválido";;
    esac
}

# Processa argumentos de linha de comando
AUTO_TARGET=""
AUTO_OUTPUT=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --target)
            AUTO_TARGET="$2"
            shift 2
            ;;
        --output)
            AUTO_OUTPUT="$2"
            shift 2
            ;;
        *)
            # Ignora outros parâmetros ou mostra ajuda
            shift
            ;;
    esac
done

check_zig

# Se o target foi fornecido, compila diretamente (modo CI / não-interativo)
if [ -n "$AUTO_TARGET" ] && [ -n "$AUTO_OUTPUT" ]; then
    compile_target "$AUTO_TARGET" "$AUTO_OUTPUT"
    exit 0
fi

# --------------------------------------------------
# Início
check_zig
compile_neonx_hash
show_menu