## build.sh
set -e
if [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32"* || "$(uname 2>/dev/null)" == *"MINGW"* || "$(uname 2>/dev/null)" == *"MSYS"* ]]; then
    NULL_DEV="NUL"
    WINDOWS_HOST="true"
else
    NULL_DEV="/dev/null"
    WINDOWS_HOST="false"
fi
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'
VERSION="DEVELOPMENT-BUILD"
BUILD_STATUS="UNOFFICIAL_BUILD"
BUILD_MAINTAINER="COMMUNITY"
SRC_DIR="src"
TOOLS_DIR="tools"
OUTPUT_DIR="build"
ZIP_DIR="bzip"
PROJECT_NAME="neonx"

if [[ "$PORTABLE" == "1" ]]; then
    TUNE_FLAGS="-march=x86-64"
    PERF_FLAGS="-O3 -fno-math-errno -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"
else
    TUNE_FLAGS="-march=native"
    PERF_FLAGS="-O3 -ffast-math -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"
fi

HARDENING_CFLAGS="-Wall -Wextra -Wconversion -Wsign-conversion -Wformat=2 -Wstrict-overflow=5 -fstack-clash-protection"
HARDENING_LDFLAGS=""
ARCH=$(uname -m)

# Aplicar flags de hardening apenas em sistemas que usam GNU ld ou LLD (Linux Desktop/Server)
# Evitar em Android/Termux e macOS onde o linker pode ser diferente ou mais restrito.
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    HARDENING_LDFLAGS="-Wl,-z,relro,-z,now -Wl,--as-needed"
    if [[ "$ARCH" == "x86_64" || "$ARCH" == "i686" ]]; then
        HARDENING_CFLAGS="$HARDENING_CFLAGS -fcf-protection=full"
    fi
fi

MATH_LIB="-lm"
if [[ "$WINDOWS_HOST" == "true" ]]; then
    PERF_FLAGS="$PERF_FLAGS"
    MATH_LIB=""
fi

print_info()    { echo -e "${CYAN}ℹ $1${NC}"; }
print_success() { echo -e "${GREEN}✓ $1${NC}"; }
print_warn()    { echo -e "${YELLOW}⚠ $1${NC}"; }
print_error()   { echo -e "${RED}✗ $1${NC}"; }

check_deps() {
    local deps=("clang" "zig" "zip")
    for tool in "${deps[@]}"; do
        if ! command -v "$tool" &> "$NULL_DEV"; then
            print_warn "Ferramenta '$tool' não encontrada. O build pode falhar dependendo da opção escolhida."
        fi
    done
}

create_zip() {
    local binary_path="$1"
    local label="$2"
    local is_windows="$3"
    if ! command -v zip &> "$NULL_DEV"; then
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

compile_wasm() {
    local label="wasm"
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build genérica: WebAssembly ($label)"
    mkdir -p "$OUTPUT_DIR" "$ZIP_DIR"
    if ! command -v emcc &> "$NULL_DEV"; then
        export PATH="$PATH:/data/data/com.termux/files/usr/opt/emscripten"
    fi
    if ! command -v emcc &> "$NULL_DEV"; then
        print_error "Emscripten (emcc) não encontrado no PATH."
        print_info "Instale via: pkg install emscripten"
        return 1
    fi
    local out_js="$OUTPUT_DIR/${PROJECT_NAME}.js"
    local out_wasm="$OUTPUT_DIR/${PROJECT_NAME}.wasm"
    emcc -O2 -s WASM=1 \
        -s EXPORTED_FUNCTIONS='["_neonx_wasm_init", "_neonx_apply_colors", "_neonx_wasm_set_frequency", "_neonx_wasm_set_opacity", "_neonx_wasm_set_quantization", "_malloc", "_free"]' \
        -s EXPORTED_RUNTIME_METHODS='["ccall", "UTF8ToString"]' \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s NO_EXIT_RUNTIME=1 \
        -g \
        -I./src \
        src/neonx_core.c \
        src/main_wasm.c \
        src/msgs.c \
        -o "$out_js"
    if [[ $? -ne 0 || ! -f "$out_js" ]]; then
        print_error "Falha na compilação: $label"
        return 1
    fi
    print_info "Criando pacote ZIP para a build: $label..."
    local zip_name="${PROJECT_NAME}_${label}.zip"
    local tmp_dir
    tmp_dir=$(mktemp -d)
    cp "$out_js" "$out_wasm" "$tmp_dir/"
    (cd "$tmp_dir" && zip -q "$zip_name" "${PROJECT_NAME}.js" "${PROJECT_NAME}.wasm")
    mv "$tmp_dir/$zip_name" "$ZIP_DIR/"
    rm -rf "$tmp_dir"
    print_success "Build $label concluída!"
}

building_tools() {
    print_info "Verificando ferramentas internas em $TOOLS_DIR/..."
    mkdir -p "$TOOLS_DIR"
    local extra_flags=""
    if [[ "$WINDOWS_HOST" == "true" ]]; then
        extra_flags="-D_CRT_SECURE_NO_WARNINGS"
    fi
    for tool_src in "$TOOLS_DIR"/*.c; do
        [[ -e "$tool_src" ]] || continue
        local tool_bin="${tool_src%.c}"
        if [[ ! -x "$tool_bin" || "$tool_src" -nt "$tool_bin" ]]; then
            print_info "Compilando ferramenta host: $(basename "$tool_src")..."
            if clang "$tool_src" -o "$tool_bin" -O2 $extra_flags $HARDENING_FLAGS 2> "$NULL_DEV"; then
                print_success "Ferramenta $(basename "$tool_bin") pronta!"
            else
                print_warn "Falha ou falta de clang ao compilar $(basename "$tool_src") (não afeta o build genérico)."
            fi
        fi
    done
}

compile_tool() {
    local target="$1"
    local bin_out="$2"
    local label="$3"
    local is_native="$4"
    mkdir -p "$OUTPUT_DIR" "$ZIP_DIR"
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build genérica: $label"
    
    # --- Início do Processo de Assinatura Efêmera ---
    local tmp_key_dir
    tmp_key_dir=$(mktemp -d)
    
    # Garantir que as ferramentas de assinatura estejam prontas
    building_tools
    
    local GENERIC_PUB_HEX=""
    if [[ -x "$TOOLS_DIR/keygen" ]]; then
        print_info "Gerando chaves efêmeras para assinatura comunitária..."
        "$TOOLS_DIR/keygen" "$tmp_key_dir/priv.key" "$tmp_key_dir/pub.key" > /dev/null
        # Usar tr -dc para pegar apenas caracteres hexadecimais, eliminando espaços/tabs/quebras de linha
        GENERIC_PUB_HEX=$(od -An -tx1 -v "$tmp_key_dir/pub.key" | tr -dc '0-9a-fA-F')
    fi
    
    local SIG_MACRO=""
    if [[ -n "$GENERIC_PUB_HEX" ]]; then
        SIG_MACRO="-DGENERIC_PUBLIC_KEY=\"$GENERIC_PUB_HEX\""
    fi
    # --- Fim do Processo de Assinatura Efêmera ---

    local final_bin="$OUTPUT_DIR/${bin_out}_${label}"
    if [[ "$label" == "native" ]]; then
        final_bin="$OUTPUT_DIR/${bin_out}"
        if [[ "$WINDOWS_HOST" == "true" ]]; then
            final_bin="${final_bin}.exe"
        fi
    fi
    local is_windows="false"
    local active_perf_flags="$PERF_FLAGS"
    if [[ "$label" == *"windows"* || ("$label" == "native" && "$WINDOWS_HOST" == "true") ]]; then
        is_windows="true"
        active_perf_flags="${active_perf_flags//-flto/}"
    fi
    local lib_prefix="lib"
    local lib_ext="a"
    if [[ "$WINDOWS_HOST" == "true" && "$is_native" == "true" ]]; then
        lib_prefix=""
        lib_ext="lib"
    fi
    local target_math_lib="$MATH_LIB"
    if [[ "$is_windows" == "true" ]]; then
        target_math_lib=""
    fi

    if [[ "$SANITIZE" == "1" ]]; then
        SANITIZER_FLAGS="-fsanitize=address,undefined -g -O1"
    else
        SANITIZER_FLAGS=""
    fi

    if [[ "$is_native" == "true" ]]; then
        clang -c "$SRC_DIR/neonx_core.c" -o "$OUTPUT_DIR/neonx_core.o" $TUNE_FLAGS $active_perf_flags $SANITIZER_FLAGS $HARDENING_CFLAGS
        ar rcs "$OUTPUT_DIR/${lib_prefix}neonx_core.${lib_ext}" "$OUTPUT_DIR/neonx_core.o" 2> "$NULL_DEV" || true
        clang "$SRC_DIR"/integrity.c "$SRC_DIR"/main.c "$SRC_DIR"/monocypher.c "$SRC_DIR"/msgs.c "$SRC_DIR"/render.c "$SRC_DIR"/shaders.c "$SRC_DIR"/terminal.c \
            "$OUTPUT_DIR/neonx_core.o" \
            -o "$final_bin" \
            $TUNE_FLAGS $active_perf_flags $DEV_KEY_MACRO $SIG_MACRO \
            -DVERSION="\"$VERSION\"" \
            -DBUILD_STATUS="\"$BUILD_STATUS\"" \
            -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            $DEV_FLAG $target_math_lib $SANITIZER_FLAGS $HARDENING_CFLAGS $HARDENING_LDFLAGS
    else
        zig cc -c "$SRC_DIR/neonx_core.c" -o "$OUTPUT_DIR/neonx_core.o" -target "$target" $active_perf_flags $SANITIZER_FLAGS $HARDENING_CFLAGS
        ar rcs "$OUTPUT_DIR/libneonx_core.a" "$OUTPUT_DIR/neonx_core.o" 2> "$NULL_DEV" || true
        zig cc "$SRC_DIR"/integrity.c "$SRC_DIR"/main.c "$SRC_DIR"/monocypher.c "$SRC_DIR"/msgs.c "$SRC_DIR"/render.c "$SRC_DIR"/shaders.c "$SRC_DIR"/terminal.c \
            -o "$final_bin" -target "$target" \
            -L"$OUTPUT_DIR" -lneonx_core \
            $active_perf_flags $DEV_KEY_MACRO $SIG_MACRO \
            -DVERSION="\"$VERSION\"" \
            -DBUILD_STATUS="\"$BUILD_STATUS\"" \
            -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            $DEV_FLAG $target_math_lib $SANITIZER_FLAGS $HARDENING_CFLAGS $HARDENING_LDFLAGS
    fi
    if [[ $? -ne 0 ]]; then
        print_error "Falha na compilação: $label"
        rm -rf "$tmp_key_dir"
        return 1
    fi

    # Assinar o binário gerado
    if [[ -x "$TOOLS_DIR/sign_binary" && -f "$tmp_key_dir/priv.key" ]]; then
        print_info "Assinando binário com chave comunitária efêmera..."
        "$TOOLS_DIR/sign_binary" "$final_bin" "$tmp_key_dir/priv.key"
    fi
    rm -rf "$tmp_key_dir"

    if [[ "$is_windows" == "true" ]]; then
        rm -f "$OUTPUT_DIR"/*.pdb 2> "$NULL_DEV" || true
    fi
    print_success "Build $label concluída!"
    create_zip "$final_bin" "$label" "$is_windows"
}

check_deps
if [[ $# -gt 0 ]]; then
    TARGET=""
    OUTPUT_BIN="$PROJECT_NAME"
    LABEL=""
    IS_NATIVE="false"
    while [[ $# -gt 0 ]]; do
        case $1 in
            --test)
                print_info "Iniciando testes unitários e de integração..."
                mkdir -p "$OUTPUT_DIR/tests"
                clang tests/unit/test_math.c src/shaders.c src/neonx_core.c src/msgs.c -o "$OUTPUT_DIR/tests/test_math" -Isrc $MATH_LIB $PERF_FLAGS $HARDENING_CFLAGS $HARDENING_LDFLAGS
                "$OUTPUT_DIR/tests/test_math"
                
                # Sempre compilar o binário nativo para testes de integração para garantir assinatura válida
                print_info "Compilando binário para testes de integração..."
                compile_tool "native" "$PROJECT_NAME" "native" "true"

                chmod +x tests/integration_test.sh
                ./tests/integration_test.sh
                exit 0
                ;;
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
    elif [[ "$TARGET" == "wasm" ]]; then
        compile_wasm
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
        echo -e "${CYAN}║${NC} 2. WebAssembly (NeonX wasm)                        ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 3. Linux x64 (Musl) via Zig                        ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 4. Linux x86 (Musl) via Zig                        ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 5. ARM64 (Android/Linux) via Zig                   ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 6. ARM32 (Android/Legacy) via Zig                  ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 7. Windows x64 via Zig                             ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 8. Windows x86 via Zig                             ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 9. MacOs arm64 via Zig                             ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 10. MacOs x64 via Zig                              ${CYAN}║${NC}"
        echo -e "${CYAN}║${NC} 11. Compilar Todas as Plataformas                  ${CYAN}║${NC}"
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
            2)
                compile_wasm
                break ;;
            [3-9]|10)
                idx=$((opt-3))
                compile_tool "${targets[$idx]}" "$PROJECT_NAME" "${labels[$idx]}" "false"
                break ;;
            11)
                compile_tool "native" "$PROJECT_NAME" "native" "true"
                compile_wasm
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
