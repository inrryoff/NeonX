import hashlib
import os
import sys
import random

def get_source_hash(base_dir):
    h = hashlib.sha256()
    source_files = []
    
    # os.walk percorre a raiz, diretórios e arquivos recursivamente
    for root, _, files in os.walk(base_dir):
        for f in files:
            if (f.endswith('.c') or f.endswith('.h')) and f != 'build_config.h':
                # Salva o caminho completo para ler o arquivo corretamente
                source_files.append(os.path.join(root, f))
    
    # Ordenar os caminhos garante que o hash seja sempre determinístico
    for filepath in sorted(source_files):
        with open(filepath, 'rb') as file:
            h.update(file.read())
    return h.hexdigest()

def generate_header(base_dir):
    s_hash = get_source_hash(base_dir)
    # Usa o hash dos códigos fontes como seed para o gerador pseudoaleatório
    random.seed(s_hash)
    
    sync_id = int(s_hash[:6], 16)
    target_sig = 0x17A2DF74
    
    # Gera 3 fragmentos aleatórios
    a = random.randint(0x01000000, 0x07000000) & 0xFFFFFF00
    b = random.randint(0x08000000, 0x14000000) & 0xFFFFFF00
    c = random.randint(0x00001000, 0x0000FFFF) & 0xFFFFFF00
    
    # 'd' é a diferença para fechar a assinatura exata
    d = (target_sig - (a + b + c)) & 0xFFFFFFFF
    
    # Garante que o header será salvo dentro de src/headers/
    header_dir = os.path.join(base_dir, 'headers')
    if not os.path.exists(header_dir):
        os.makedirs(header_dir)
        
    header_path = os.path.join(header_dir, 'build_config.h')
    
    with open(header_path, 'w') as f:
        f.write('/* Auto-generated build configuration - DO NOT EDIT */\n')
        f.write('#ifndef BUILD_CONFIG_H\n#define BUILD_CONFIG_H\n\n')
        f.write(f'#define NX_SYNC_ID ((uint32_t)0x{sync_id:06X}UL)\n')
        f.write(f'#define NX_AUTH_SIG ((uint32_t)0x{target_sig:08X}UL)\n')
        f.write(f'#define NX_FRAGMENT_A ((uint32_t)0x{a:08X}UL)\n')
        f.write(f'#define NX_FRAGMENT_B ((uint32_t)0x{b:08X}UL)\n')
        f.write(f'#define NX_FRAGMENT_C ((uint32_t)0x{c:08X}UL)\n')
        f.write(f'#define NX_FRAGMENT_D ((uint32_t)0x{d:08X}UL)\n')
        f.write('\n#endif\n')
    
    print(f"Generated {header_path} with SyncID: {sync_id:06X}")

if __name__ == "__main__":
    # O build.sh passa "src" como argumento via $SRC_DIR
    src_path = 'src'
    if len(sys.argv) > 1:
        src_path = sys.argv[1]
    generate_header(src_path)
