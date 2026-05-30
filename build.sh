
#!/usr/bin/env bash
set -e

# =====================================================
# NeonX Builder – Community Edition (Forks & Dev)
# =====================================================

# --------------------------------------------------
# Portabilidade de Dispositivo Nulo
# --------------------------------------------------
if [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32"* || "$(uname 2>/dev/null)" == *"MINGW"* ]]; then
    NULL_DEV="NUL"
    WINDOWS_HOST="true"
else
    NULL_DEV="/dev/null"
    WINDOWS_HOST="false"
fi

# Cores
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

print_info()    { echo -e "${CYAN}ℹ $1${NC}"; }
print_success() { echo -e "${GREEN}✓ $1${NC}"; }
print_warn()    { echo -e "${YELLOW}⚠ $1${NC}"; }
print_error()   { echo -e "${RED}✗ $1${NC}"; }

# --------------------------------------------------
# Gestão de Versão Local (Sem chamadas de rede)
# --------------------------------------------------
VERSION_FILE="version.txt"
if [[ -f "$VERSION_FILE" ]]; then
    VERSION=$(cat "$VERSION_FILE" | tr -d '[:space:]')
    print_info "Versão local carregada: $VERSION"
else
    VERSION="DEVELOPMENT-BUILD"
    echo "$VERSION" > "$VERSION_FILE"
    print_warn "Arquivo $VERSION_FILE não encontrado. Usando padrão: $VERSION"
fi

BUILD_STATUS="UNOFFICIAL_BUILD"
BUILD_MAINTAINER="@community"
MAINTAINER_CLEAN=$(echo "$BUILD_MAINTAINER" | tr -d '@' | tr 'a-z' 'A-Z')
BUILD_STATUS="VALID_SIG_BY_${MAINTAINER_CLEAN}"

SRC_DIR="src"
TOOLS_DIR="tools"
OUTPUT_DIR="build"
ZIP_DIR="bzip"
KEYS_DIR="keys"
PROJECT_NAME="neonx"
HASH_FILE="$KEYS_DIR/SHA256SUMS.txt"

mkdir -p "$KEYS_DIR"
# Busca a primeira chave disponível no diretório, ou fica vazio se não existir
INTERNAL_KEY=$(ls "$KEYS_DIR"/*.key 2>/dev/null | head -n 1)

# --------------------------------------------------
# Flags de Compilação & Performance
# --------------------------------------------------
if [[ "$PORTABLE" == "1" ]]; then
    TUNE_FLAGS="-march=x86-64"
    PERF_FLAGS="-O3 -fno-math-errno -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"
else
    TUNE_FLAGS="-march=native"
    PERF_FLAGS="-O3 -ffast-math -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wno-unused-result"
fi

HARDENING_CFLAGS="-Wall -Wextra -Wconversion -Wsign-conversion -Wformat=2 -Wno-format-nonliteral -Wstrict-overflow=5 -Wno-unused-command-line-argument"
if [[ "$OSTYPE" == "linux-gnu"* || "$OSTYPE" == "linux-android"* ]]; then
    HARDENING_CFLAGS="$HARDENING_CFLAGS -fstack-clash-protection"
fi

HARDENING_LDFLAGS=""
ARCH=$(uname -m)
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    HARDENING_LDFLAGS="-Wl,-z,relro,-z,now -Wl,--as-needed"
    if [[ "$ARCH" == "x86_64" || "$ARCH" == "i686" ]]; then
        HARDENING_CFLAGS="$HARDENING_CFLAGS -fcf-protection=full"
    fi
fi

MATH_LIB="-lm"
if [[ "$WINDOWS_HOST" == "true" ]]; then
    MATH_LIB=""
fi

SAN_FLAGS=""
if [[ "$SANITIZE" == "1" ]]; then
    print_info "MODO SANITIZE ATIVADO"
    SAN_FLAGS="-fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer"
    PERF_FLAGS="${PERF_FLAGS//-flto/}"
fi

# Compatibilidade com Bash < 4.0 (macOS padrão)
BUILD_REPORT_KEYS=()
BUILD_REPORT_VALUES=()
update_build_report() {
    local label="$1"
    local status="$2"
    local found=false
    for i in "${!BUILD_REPORT_KEYS[@]}"; do
        if [[ "${BUILD_REPORT_KEYS[$i]}" == "$label" ]]; then
            BUILD_REPORT_VALUES[$i]="$status"
            found=true
            break
        fi
    done
    if [[ "$found" == false ]]; then
        BUILD_REPORT_KEYS+=("$label")
        BUILD_REPORT_VALUES+=("$status")
    fi
}

check_deps() {
    local deps=("clang" "zig" "hexdump" "zip" "sha256sum")
    for tool in "${deps[@]}"; do
        if ! command -v "$tool" &> "$NULL_DEV"; then
            print_warn "Ferramenta '$tool' não encontrada."
        fi
    done
}

clean_old_builds() {
    print_info "Realizando limpeza dos diretórios..."
    rm -rf "$OUTPUT_DIR" "$ZIP_DIR" "$KEYS_DIR"/*.txt 2> "$NULL_DEV" || true
    mkdir -p "$OUTPUT_DIR" "$ZIP_DIR" "$KEYS_DIR"
}

sign_binary() {
    local binary="$1"
    local priv_key="$2"

    if [[ "$binary" != *".wasm" ]]; then
        if [[ -f "$priv_key" && -x "$TOOLS_DIR/sign_binary" ]]; then
            print_info "Aplicando Assinatura Interna..."
            local SIG_HEX=$("$TOOLS_DIR/sign_binary" "$binary" "$priv_key")
            echo -n "$SIG_HEX" >> "$binary"
            print_success "Assinatura interna comunitária anexada!"
        else
            print_warn "Ferramentas ou chave ausentes. Pulando assinatura."
        fi
    fi
}

update_hash() {
    local binary_path="$1"
    local bin_name=$(basename "$binary_path")
    touch "$HASH_FILE"
    grep -v "$bin_name" "$HASH_FILE" > "${HASH_FILE}.tmp" || true
    mv "${HASH_FILE}.tmp" "$HASH_FILE"
    (cd "$OUTPUT_DIR" && sha256sum "$bin_name") >> "$HASH_FILE"
}

create_zip() {
    local binary_path="$1"
    local label="$2"
    local is_windows="$3"

    if ! command -v zip &> "$NULL_DEV"; then return 1; fi

    local zip_name="${PROJECT_NAME}_${label}.zip"
    local tmp_dir=$(mktemp -d)

    if [[ "$binary_path" == *".wasm" ]]; then
        local base_path="${binary_path%.wasm}"
        cp "$binary_path" "${base_path}.js" "$tmp_dir/" 2> "$NULL_DEV" || true
        (cd "$tmp_dir" && zip -q "$zip_name" *)
    else
        local internal_bin_name="$PROJECT_NAME"
        [[ "$is_windows" == "true" ]] && internal_bin_name="${PROJECT_NAME}.exe"
        cp "$binary_path" "$tmp_dir/$internal_bin_name"
        (cd "$tmp_dir" && zip -q "$zip_name" *)
    fi

    mv "$tmp_dir/$zip_name" "$ZIP_DIR/"
    rm -rf "$tmp_dir"
}

validate_native_binary() {
    local bin="$1"
    if [[ -f "$bin" ]]; then
        chmod +x "$bin"
        if "$bin" --help > "$NULL_DEV" 2>&1 || [[ $? -eq 0 || $? -eq 2 ]]; then return 0; fi
    fi
    return 1
}

compile_wasm() {
    local label="wasm"
    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build: WebAssembly ($label)"

    if ! command -v emcc &> "$NULL_DEV"; then
        export PATH="$PATH:/data/data/com.termux/files/usr/opt/emscripten"
    fi

    if ! command -v emcc &> "$NULL_DEV"; then
        update_build_report "$label" "FALHOU (SEM EMCC)"
        return 1
    fi

    local out_js="$OUTPUT_DIR/${PROJECT_NAME}.js"
    local out_wasm="$OUTPUT_DIR/${PROJECT_NAME}.wasm"

    # Removido o "-g" para silenciar o warning do binaryen e permitir O2 full
    emcc -O2 -s WASM=1 \
        -s EXPORTED_FUNCTIONS='["_neonx_wasm_init", "_neonx_apply_colors", "_neonx_wasm_set_frequency", "_neonx_wasm_set_opacity", "_neonx_wasm_set_quantization", "_malloc", "_free"]' \
        -s EXPORTED_RUNTIME_METHODS='["ccall", "UTF8ToString"]' \
        -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -I./src \
        src/math_fixed.c src/shader_effects.c src/render_core.c src/main_wasm.c src/msgs.c \
        -o "$out_js"

    if [[ $? -ne 0 || ! -f "$out_js" ]]; then
        update_build_report "$label" "FALHOU"
        return 1
    fi

    update_hash "$out_wasm"
    update_hash "$out_js"
    create_zip "$out_wasm" "$label" "false"
    update_build_report "$label" "SUCESSO"
    print_success "Build $label concluída!"
}

compile_tool() {
    local target="$1"
    local bin_out="$2"
    local label="$3"
    local is_native="$4"

    echo -e "${YELLOW}--------------------------------------------------${NC}"
    print_info "Iniciando build: $label"

    local SIG_MACRO=""
    # Lógica de Chave Comunitária Persistente
    if [[ -z "$INTERNAL_KEY" ]]; then
        print_info "Nenhuma chave encontrada. Gerando chave comunitária permanente..."
        "$TOOLS_DIR/keygen" "$KEYS_DIR/community.key" "$KEYS_DIR/community.pub" > "$NULL_DEV" 2>&1
        INTERNAL_KEY="$KEYS_DIR/community.key"
        print_success "Chave gerada em $INTERNAL_KEY"
    fi

    if [[ -f "$INTERNAL_KEY" ]]; then
        local pub_key="${INTERNAL_KEY%.key}.pub"
        if [[ -f "$pub_key" ]]; then
            local GENERIC_PUB_HEX=$("$TOOLS_DIR/keygen" "$INTERNAL_KEY" "$pub_key" --print-hex | tail -n 1 | tr -dc '0-9a-fA-F')
            SIG_MACRO="-DGENERIC_NEONX_KEY=\"$GENERIC_PUB_HEX\""
        fi
    fi

    local final_bin="$OUTPUT_DIR/${bin_out}_${label}"
    [[ "$label" == "native" || "$label" == "Cortex-A75" ]] && final_bin="$OUTPUT_DIR/${bin_out}"

    local is_windows="false"
    local active_perf_flags="$PERF_FLAGS"
    local target_math_lib="$MATH_LIB"

    # Adicionado -lbcrypt para o Windows via Zig
    if [[ "$label" == *"windows"* || ("$is_native" == "true" && "$WINDOWS_HOST" == "true") ]]; then
        is_windows="true"
        active_perf_flags="${active_perf_flags//-flto/} -D_CRT_SECURE_NO_WARNINGS"
        target_math_lib="-lbcrypt"
        final_bin="${final_bin}.exe"
    fi

    local obj_math="$OUTPUT_DIR/math_fixed_${label}.o"
    local obj_seff="$OUTPUT_DIR/shader_effects_${label}.o"
    local obj_rend="$OUTPUT_DIR/render_core_${label}.o"
    local core_lib_name="libneonx_core_${label}.a"
    local core_lib_flag="neonx_core_${label}"

    if [[ "$is_windows" == "true" ]]; then
        core_lib_name="neonx_core_${label}.lib"
    fi
    local core_a="$OUTPUT_DIR/$core_lib_name"

    if [[ "$is_native" == "true" ]]; then
        clang -c "$SRC_DIR/math_fixed.c" -o "$obj_math" $TUNE_FLAGS $active_perf_flags $SAN_FLAGS $HARDENING_CFLAGS
        clang -c "$SRC_DIR/shader_effects.c" -o "$obj_seff" $TUNE_FLAGS $active_perf_flags $SAN_FLAGS $HARDENING_CFLAGS
        clang -c "$SRC_DIR/render_core.c" -o "$obj_rend" $TUNE_FLAGS $active_perf_flags $SAN_FLAGS $HARDENING_CFLAGS
        
        ar rcs "$core_a" "$obj_math" "$obj_seff" "$obj_rend" 2> "$NULL_DEV" || true

        clang "$SRC_DIR"/integrity.c "$SRC_DIR"/main.c "$SRC_DIR"/monocypher.c "$SRC_DIR"/msgs.c "$SRC_DIR"/render.c "$SRC_DIR"/shaders.c "$SRC_DIR"/terminal.c \
            -o "$final_bin" \
            -L"$OUTPUT_DIR" -l"$core_lib_flag" \
            $TUNE_FLAGS $active_perf_flags $SAN_FLAGS \
            $SIG_MACRO \
            -DVERSION="\"$VERSION\"" -DBUILD_STATUS="\"$BUILD_STATUS\"" -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            $target_math_lib $HARDENING_CFLAGS $HARDENING_LDFLAGS
    else
        zig cc -c "$SRC_DIR/math_fixed.c" -o "$obj_math" -target "$target" $active_perf_flags $SAN_FLAGS $HARDENING_CFLAGS
        zig cc -c "$SRC_DIR/shader_effects.c" -o "$obj_seff" -target "$target" $active_perf_flags $SAN_FLAGS $HARDENING_CFLAGS
        zig cc -c "$SRC_DIR/render_core.c" -o "$obj_rend" -target "$target" $active_perf_flags $SAN_FLAGS $HARDENING_CFLAGS
        
        zig ar rcs "$core_a" "$obj_math" "$obj_seff" "$obj_rend" 2> "$NULL_DEV" || true

        zig cc "$SRC_DIR"/integrity.c "$SRC_DIR"/main.c "$SRC_DIR"/monocypher.c "$SRC_DIR"/msgs.c "$SRC_DIR"/render.c "$SRC_DIR"/shaders.c "$SRC_DIR"/terminal.c \
            -o "$final_bin" -target "$target" \
            -L"$OUTPUT_DIR" -l"$core_lib_flag" \
            $active_perf_flags \
            $SIG_MACRO \
            -DVERSION="\"$VERSION\"" -DBUILD_STATUS="\"$BUILD_STATUS\"" -DBUILD_MAINTAINER="\"$BUILD_MAINTAINER\"" \
            $target_math_lib $HARDENING_CFLAGS $HARDENING_LDFLAGS
    fi

    if [[ $? -ne 0 ]]; then
        update_build_report "$label" "FALHOU"
        return 1
    fi

    [[ "$is_windows" == "true" ]] && rm -f "$OUTPUT_DIR"/*.pdb 2> "$NULL_DEV" || true

    if [[ "$is_native" == "true" ]]; then
        if ! validate_native_binary "$final_bin"; then
            update_build_report "$label" "CRASH"
            return 1
        fi
    fi

    sign_binary "$final_bin" "$INTERNAL_KEY"
    update_hash "$final_bin"
    create_zip "$final_bin" "$label" "$is_windows"
    
    update_build_report "$label" "SUCESSO"
    print_success "Build $label concluída!"
}

building_tools() {
    mkdir -p "$TOOLS_DIR"
    for tool_src in "$TOOLS_DIR"/*.c; do
        [[ -e "$tool_src" ]] || continue
        local tool_bin="${tool_src%.c}"
        if [[ ! -x "$tool_bin" || "$tool_src" -nt "$tool_bin" ]]; then
            clang "$tool_src" -o "$tool_bin" -O2 2> "$NULL_DEV" || exit 1
        fi
    done
}

check_deps
clean_old_builds
building_tools

targets=("x86_64-linux-musl" "x86-linux-musl" "aarch64-linux-musl" "arm-linux-musleabihf" "x86_64-windows-gnu" "x86-windows-gnu" "aarch64-macos" "x86_64-macos")
labels=("linux-x64" "linux-x86" "linux-arm64" "linux-arm32" "windows-x64" "windows-x86" "macos-arm64" "macos-x64")

if [[ $# -gt 0 ]]; then
    while [[ $# -gt 0 ]]; do
        case $1 in
            --test)
                mkdir -p "$OUTPUT_DIR/tests"
                clang tests/unit/test_comprehensive.c src/shaders.c src/math_fixed.c src/shader_effects.c src/render_core.c src/msgs.c -o "$OUTPUT_DIR/tests/test_unit" -Isrc $MATH_LIB $PERF_FLAGS
                "$OUTPUT_DIR/tests/test_unit"
                
                compile_tool "native" "$PROJECT_NAME" "native" "true"
                [[ -f tests/integration_test.sh ]] && chmod +x tests/integration_test.sh && ./tests/integration_test.sh
                exit 0
                ;;
            --test-debug)
                mkdir -p "$OUTPUT_DIR/tests"
                print_info "Compilando Testes em Modo Debug (Verbose)..."
                clang tests/unit/test_comprehensive.c src/shaders.c src/math_fixed.c src/shader_effects.c src/render_core.c src/msgs.c -DVERBOSE_DEBUG -o "$OUTPUT_DIR/tests/test_debug" -Isrc $MATH_LIB $PERF_FLAGS
                "$OUTPUT_DIR/tests/test_debug"
                exit 0
                ;;
            --native) compile_tool "native" "$PROJECT_NAME" "native" "true"; exit 0 ;;
            --wasm) compile_wasm; exit 0 ;;
        esac
        shift
    done
fi

show_menu() {
    local largura_interna=50
    local cabecalho="${YELLOW}NeonX Builder v4.6 (Community Edition)${NC}"
    local espacos_cabecalho=$((largura_interna - ${#cabecalho} - 8))
    local linha_cabecalho=$(printf "${CYAN}║${NC} %s%*s${CYAN}║" "$cabecalho" $espacos_cabecalho "")
    local texto_versao_puro="Versão Local: $VERSION"
    local espacos_versao=$((largura_interna - ${#texto_versao_puro} + 1))
    local linha_versao=$(printf "${CYAN}║${NC} Versão Local: ${GREEN}%s${NC}%*s${CYAN}║" "$VERSION" $espacos_versao "")
    
    echo -e "\n${CYAN}╔════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}${linha_cabecalho}${NC}"
    echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
    echo -e "${CYAN}${linha_versao}${NC}"
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

while true; do
    show_menu
    read -p "Opção: " opt
    case $opt in
        1) compile_tool "native" "$PROJECT_NAME" "native" "true"; break ;;
        2) compile_wasm; break ;;
        [3-9]|10) idx=$((opt-3)); compile_tool "${targets[$idx]}" "$PROJECT_NAME" "${labels[$idx]}" "false"; break ;;
        11)
            compile_tool "native" "$PROJECT_NAME" "native" "true"
            compile_wasm
            for i in "${!targets[@]}"; do compile_tool "${targets[$i]}" "$PROJECT_NAME" "${labels[$i]}" "false"; done
            break ;;
        0) exit 0 ;;
        *) print_error "Opção inválida!"; sleep 1 ;;
    esac
done

largura_interna=50
echo -e "\n${CYAN}╔════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║             ${YELLOW}RELATÓRIO DE BUILD NEONX${NC}               ${CYAN}║${NC}"
echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
for i in "${!BUILD_REPORT_KEYS[@]}"; do
    key="${BUILD_REPORT_KEYS[$i]}"
    status="${BUILD_REPORT_VALUES[$i]}"
    if [[ "$status" == "SUCESSO" ]]; then
        printf "${CYAN}║${NC}  ├── %-26s: ${GREEN}%-14s${NC}    ${CYAN}║${NC}\n" "$key" "$status"
    else
        printf "${CYAN}║${NC}  ├── %-26s: ${RED}%-14s${NC}      ${CYAN}║${NC}\n" "$key" "$status"
    fi
done
echo -e "${CYAN}╠════════════════════════════════════════════════════╣${NC}"
echo -e "${CYAN}║  🎉 Processo finalizado!                           ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════╝${NC}\n"
