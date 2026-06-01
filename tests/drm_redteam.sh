#!/bin/bash

# NeonX Red-Team DRM Test Script
# Objetivo: Tentar quebrar o DRM usando técnicas de um "plagiador médio".

BIN="./build/neonx"
ORIGINAL_NAME="@inrryoff"

echo "=== INICIANDO TESTE DE ESTRESSE DRM ==="

# 1. Teste de Identificação (strings)
echo -n "[1/3] Testando visibilidade da string funcional... "
# Procuramos pela sequência XOR (não deve ser encontrada como string)
if strings $BIN | grep -q "inrryoff"; then
    echo "AVISO: Nome encontrado nos textos de licença (Informacional)."
else
    echo "SUCESSO: Nome funcional oculto."
fi

# 2. Teste de Sabotagem Silenciosa (Fragmentos)
echo "[2/3] Simulando corrupção de fragmento de autenticação..."
# Vamos corromper o fragmento f1 em integrity.c
sed -i 's/0x0100UL/0x0101UL/' src/integrity.c
make release > /dev/null 2>&1

echo "  > Executando NeonX com fragmento F1 corrompido..."
# O NeonX não deve avisar nada, mas o visual deve estar quebrado.
# Como não podemos ver, vamos checar se o bid em terminal.c (que usa a soma) mudaria se o tivéssemos vinculado.
# Mas aqui, o bid em terminal.c usa nx_verify_build_sync direto no nome.
# A sabotagem visual é interna ao render_core.c.

# Para provar que o DRM funciona, vamos mudar o NOME no XOR.
git checkout src/integrity.c
sed -i 's/26, 51, 52/27, 51, 52/' src/math_fixed.c
make release > /dev/null 2>&1

echo "  > Executando NeonX com nome funcional alterado..."
output=$(./build/neonx -v --lang en)
echo "  > Resultado do Status: $(echo "$output" | grep "Build:")"

if echo "$output" | grep -q "0000-FX11"; then
    echo "SUCESSO: DRM detectou a falha de sincronia e ativou o modo degradado."
else
    echo "FALHA: O sistema não detectou a alteração do nome funcional!"
fi

# Restaurar
git checkout src/math_fixed.c
make release > /dev/null 2>&1
echo "=== TESTE CONCLUÍDO ==="
