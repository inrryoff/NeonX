#!/usr/bin/env bash
set -e

# =====================================================
# NeonX Builder – CI & Generic Developer Edition
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
VERSION="DEVELOPMENT-BUILD"
BUILD_STATUS="UNOFFICIAL_BUILD"
BUILD_MAINTAINER="COMMUNITY"

# Diretórios
SRC_DIR="src"
TOOLS_DIR="tools"
OUTPUT_DIR="build"
ZIP_DIR="bzip"
PROJECT_NAME="neonx"

# --------------------------------------------------
# Flags de Compilação & Performance
# --------------------------------------------------
TUNE_FLAGS="-march=native"
PERF_FLAGS="-O3 -ffast-math -flto -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"

# --------------------------------------------------
# Funções Auxiliares
# --------------------------------------------------
print_info()    { echo -e "${CYAN}ℹ $1${NC}"; }
print_success() { echo -e "${GREEN}✓ $1${NC}"; }
print_warn()    { echo -e "${YELLOW}⚠ $1${NC}"; }
print_error()   { echo -e "${RED}✗ $1${NC}"; }

check_deps() {
    local deps=("clang" "zig" "zip")
    for tool in "${deps[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            print_warn "Ferramenta '$tool' não encontrada. O build pode falhar dependendo da opção escolhida."
        fi
    done
}

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

    if [[ "$is_windows" == "true" ]]; then
        internal_bin_name="${PROJECT_NAME}.exe"
    fi

    local zip_name="${PROJECT_NAME}_${label}.zip"
    local tmp_dir
    tmp_dir=$(mktemp -d)
    cp "$binary_path" "$tmp_dir/$internal_bin_name"

    (cd "$tmp_dir" && zip -q "$zip_name" "$internal_bin_name")
    mv "$tmp_dir/$zip_name" "$ZIP_DIR/"
    rm -rf "$tmp_dir"
    print_success "Arquivo gerado com sucesso: $ZIP_DIR/$zip_name"
}

# --------------------------------------------------
# Compilação de Ferramentas Auxiliares
# --------------------------------------------------
building_tools() {
    print_info "Verificando ferramentas internas em $TOOLS_DIR/..."
    mkdir -p "$TOOLS_DIR"
    for tool_src in "$TOOLS_DIR"/*.c; do
        [[ -e "$tool_src" ]] || continue
        local tool_bin="${tool_src%.c}"
        if [[ ! -x "$tool_bin" || "$tool_src" -nt "$tool_bin" ]]; then
            print_info "Compilando ferramenta host: $(basename "$tool_src")..."
            if clang "$tool_src" -o "$tool_bin" -O2 2>/dev/null; then
                print_success "Ferramenta $(basename "$tool_bin") pronta!"
            else
                print_warn "Falha ou falta de clang ao compilar $(basename "$tool_src") (não afeta o build genérico)."
            fi
        fi
    done
}

# --------------------------------------------------
# Núcleo de Compilação
# --------------------------------------------------
compile_tool() {
    local target="$1"
    local bin_out="$2"
    local label="$3"
    local is_native="$4"
    mkdir -p "$OUTPUT_DIR" "$ZIP_DIR"
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build genérica: $label"
    local final_bin="$OUTPUT_DIR/${bin_out}_${label}"
    if [[ "$label" == "native" ]]; then
        final_bin="$OUTPUT_DIR/${bin_out}"
    fi

    local is_windows="false"
    local active_perf_flags="$PERF_FLAGS"

    if [[ "$label" == *"windows"* ]]; then
        is_windows="true"
        active_perf_flags="${active_perf_flags//-flto/}"
    fi

    if [[ "$is_native" == "true" ]]; then
        clang "$SRC_DIR"/*.c -o "$final_bin" \
            $TUNE_FLAGS $active_perf_flags $DEV_KEY_MACRO \
            -DVERSION="\"$VERSION\"" \
            -DBUILD_STATUS="\"$BUILD_STATUS\"" \
            -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            $DEV_FLAG -lm
    else
        zig cc "$SRC_DIR"/*.c -o "$final_bin" -target "$target" \
            $active_perf_flags $DEV_KEY_MACRO \
            -DVERSION="\"$VERSION\"" \
            -DBUILD_STATUS="\"$BUILD_STATUS\"" \
            -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            $DEV_FLAG -lm
    fi

    if [[ $? -ne 0 ]]; then
        print_error "Falha na compilação: $label"
        return 1
    fi

    if [[ "$is_windows" == "true" ]]; then
        rm -f "$OUTPUT_DIR"/*.pdb 2>/dev/null || true
    fi

    print_success "Build $label concluída!"
    create_zip "$final_bin" "$label" "$is_windows"
}

# --------------------------------------------------
# Modo de Execução (CI vs Interativo)
# --------------------------------------------------
check_deps

if [[ $# -gt 0 ]]; then
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

    if [[ "$IS_NATIVE" == "true" ]]; then
        compile_tool "native" "$OUTPUT_BIN" "$LABEL" "true"
    elif [[ -n "$TARGET" && -n "$LABEL" ]]; then
        compile_tool "$TARGET" "$OUTPUT_BIN" "$LABEL" "false"
    else
        print_error "Nenhum alvo especificado. O CI precisa passar --native ou --target."
        exit 1
    fi

else
    building_tools

    show_menu() {
        echo -e "\n${CYAN}╔════════════════════════════════════════════════════╗${NC}"
        echo -e "${CYAN}║${NC} ${YELLOW}NeonX Dev Builder (Clang Native & Zig Cross)       ${NC}${CYAN}║${NC}"
        echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
        echo -e "${CYAN}║${NC} 1. Build Nativa (Local via Clang)                  ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 2. Linux x64 (Musl) via Zig                        ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 3. Linux x86 (Musl) via Zig                        ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 4. ARM64 (Android/Linux) via Zig                   ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 5. ARM32 (Android/Legacy) via Zig                  ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 6. Windows x64 via Zig                             ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 7. Windows x86 via Zig                             ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 8. MacOs arm64 via Zig                             ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 9. MacOs x64 via Zig                               ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 10. Compilar Todas as Plataformas                  ${CYAN}║${NC}"
        echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
        echo -e "${CYAN}║${NC} 0. Sair                                            ${CYAN}║${NC}"
        echo -e "${CYAN}╚════════════════════════════════════════════════════╝${NC}"
    }

    targets=("x86_64-linux-musl" "x86-linux-musl" "aarch64-linux-musl" "arm-linux-musleabihf" "x86_64-windows-gnu" "x86-windows-gnu" "aarch64-macos" "x86_64-macos")
    labels=("linux-x64" "linux-x86" "linux-arm64" "linux-arm32" "windows-x64" "windows-x86" "macos-arm64" "macos-x64")

    while true; do
        show_menu
        read -p "Opção: " opt
        case $opt in
            1)
                compile_tool "native" "$PROJECT_NAME" "native" "true"
                break ;;
            [2-9])
                idx=$((opt-2))
                compile_tool "${targets[$idx]}" "$PROJECT_NAME" "${labels[$idx]}" "false"
                break ;;
            10)
                compile_tool "native" "$PROJECT_NAME" "native" "true"
                for i in "${!targets[@]}"; do
                    compile_tool "${targets[$i]}" "$PROJECT_NAME" "${labels[$i]}" "false"
                done
                break ;;
            0) exit 0 ;;
            *) print_error "Opção inválida!"; sleep 1 ;;
        esac
    done

    echo -e "\n${GREEN}Processo finalizado. Verifique as pastas $OUTPUT_DIR/ e $ZIP_DIR/${NC}"
fi