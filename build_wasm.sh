#!/usr/bin/env bash
# Script para compilar o NeonX para WebAssembly usando Emscripten

# Tenta localizar emcc se não estiver no PATH (comum no Termux)
if ! command -v emcc &> /dev/null; then
    export PATH="$PATH:/data/data/com.termux/files/usr/opt/emscripten"
fi

if ! command -v emcc &> /dev/null; then
    echo "Erro: Emscripten (emcc) não encontrado no PATH."
    echo "Instale via: pkg install emscripten"
    exit 1
fi

echo "Compilando NeonX Core para WebAssembly..."

# -O3 para performance máxima
# -s WASM=1 habilita WebAssembly
# EXPORTED_FUNCTIONS define quais funções C podem ser chamadas pelo JS
# EXPORTED_RUNTIME_METHODS adiciona utilitários como cwrap (substitui EXTRA_EXPORTED_RUNTIME_METHODS)
emcc -O3 -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_neonx_wasm_init", "_neonx_wasm_render_canvas", "_neonx_apply_colors", "_neonx_wasm_set_frequency", "_neonx_wasm_set_gradient_angle", "_neonx_wasm_set_opacity", "_neonx_wasm_set_quantization", "_malloc", "_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "UTF8ToString"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -I./src src/neonx_core.c src/neonx_wasm.c -o neonx.js

if [ $? -eq 0 ]; then
    echo "Sucesso! Arquivos gerados: neonx.js, neonx.wasm"
else
    echo "Falha na compilação WASM."
    exit 1
fi
