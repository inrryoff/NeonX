#!/usr/bin/env bash
set -e

# =====================================================
# NeonX Builder – CI / Generic Edition
# =====================================================

# Cores
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

# --------------------------------------------------
# Configurações de Identidade (Versão Genérica)
# --------------------------------------------------
VERSION="CI-BUILD"
BUILD_STATUS="UNOFFICIAL_CI_BUILD"
BUILD_MAINTAINER="CI_AUTOMATION"

# Diretórios
SRC_DIR="src"
OUTPUT_DIR="build"
PROJECT_NAME="neonx"

# --------------------------------------------------
# Flags de Compilação & Performance
# --------------------------------------------------
TUNE_FLAGS="-march=native"
PERF_FLAGS="-O3 -ffast-math -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"

# --------------------------------------------------
# Funções Auxiliares
# --------------------------------------------------
print_info()    { echo -e "${CYAN}ℹ $1${NC}"; }
print_success() { echo -e "${GREEN}✓ $1${NC}"; }
print_warn()    { echo -e "${YELLOW}⚠ $1${NC}"; }
print_error()   { echo -e "${RED}✗ $1${NC}"; }

# --------------------------------------------------
# Geração do Pacote ZIP
# --------------------------------------------------
create_zip() {
    local binary_path="$1"
    local label="$2"
    local is_windows="$3"
    
    if ! command -v zip &> /dev/null; then
        print_error "Comando 'zip' não instalado. Não foi possível gerar o pacote comprimido."
        return 1
    fi

    print_info "Criando pacote ZIP para a build: $label..."
    local internal_bin_name="$PROJECT_NAME"
    if [ "$is_windows" == "true" ]; then
        internal_bin_name="${PROJECT_NAME}.exe"
    fi
    local zip_name="${PROJECT_NAME}_${label}.zip"
    local sig_name="${PROJECT_NAME}_${label}.minisig"
    local tmp_dir
    tmp_dir=$(mktemp -d)
    cp "$binary_path" "$tmp_dir/$internal_bin_name"
    if [ -f "${binary_path}.minisig" ]; then
        cp "${binary_path}.minisig" "$tmp_dir/$sig_name"
    fi
    (cd "$tmp_dir" && zip -q "$zip_name" "$internal_bin_name" $([ -f "$sig_name" ] && echo "$sig_name"))
    mv "$tmp_dir/$zip_name" "$OUTPUT_DIR/"
    rm -rf "$tmp_dir"
    print_success "Arquivo gerado com sucesso: $OUTPUT_DIR/$zip_name"
}

# --------------------------------------------------
# Núcleo de Compilação (Limpo de Chaves)
# --------------------------------------------------
compile_tool() {
    local target="$1"
    local bin_out="$2"
    local label="$3"
    local is_native="$4"

    mkdir -p "$OUTPUT_DIR"
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build: $label"
    
    local final_bin="$OUTPUT_DIR/${bin_out}_${label}"

    local is_windows="false"
    if [[ "$label" == *"windows"* ]]; then
        is_windows="true"
    fi

    if [ "$is_native" == "true" ]; then
        clang "$SRC_DIR"/*.c -o "$final_bin" \
            $TUNE_FLAGS $PERF_FLAGS \
            -DVERSION="\"$VERSION\"" \
            -DBUILD_STATUS="\"$BUILD_STATUS\"" \
            -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            -lm
    else
        zig cc "$SRC_DIR"/*.c -o "$final_bin" -target "$target" \
            $PERF_FLAGS \
            -DVERSION="\"$VERSION\"" \
            -DBUILD_STATUS="\"$BUILD_STATUS\"" \
            -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            -lm
    fi

    if [ $? -ne 0 ]; then
        print_error "Falha na compilação: $label"
        exit 1
    fi

    print_success "Build $label concluída!"
    create_zip "$final_bin" "$label" "$is_windows"
}

# --------------------------------------------------
# Parseador de Argumentos
# --------------------------------------------------
TARGET=""
OUTPUT_BIN="$PROJECT_NAME"
LABEL=""
IS_NATIVE="false"

while [[ $# -gt 0 ]]; do
    case $1 in
        --native)
            IS_NATIVE="true"
            LABEL="native"
            shift
            ;;
        --target)
            TARGET="$2"
            shift 2
            ;;
        --output)
            OUTPUT_BIN="$2"
            shift 2
            ;;
        --label)
            LABEL="$2"
            shift 2
            ;;
        *)
            print_error "Argumento inválido: $1"
            echo "Uso: ./build.sh --native  OU  ./build.sh --target <alvo> --output <nome> --label <nome>"
            exit 1
            ;;
    esac
done

if [ "$IS_NATIVE" == "true" ]; then
    compile_tool "native" "$OUTPUT_BIN" "$LABEL" "true"
elif [ -n "$TARGET" ] && [ -n "$LABEL" ]; then
    compile_tool "$TARGET" "$OUTPUT_BIN" "$LABEL" "false"
else
    print_error "Nenhum alvo especificado. O CI precisa passar --native ou --target."
    exit 1
fi