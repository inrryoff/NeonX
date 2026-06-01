import hashlib
import os
import sys
import random

def get_source_hash(src_dir):
    h = hashlib.sha256()
    files = []
    for f in os.listdir(src_dir):
        if f.endswith('.c') or f.endswith('.h'):
            if f != 'build_config.h':
                files.append(f)
    
    for f in sorted(files):
        with open(os.path.join(src_dir, f), 'rb') as file:
            h.update(file.read())
    return h.hexdigest()

def generate_header(src_dir):
    s_hash = get_source_hash(src_dir)
    # Use the source hash to seed the random generator for this build
    random.seed(s_hash)
    
    sync_id = int(s_hash[:6], 16)
    
    # Target sum: hash of "@inrryoff"
    target_sig = 0x17A2DF74
    
    # Generate 3 random fragments
    # We keep them in similar ranges to the originals to avoid suspicion
    # a ~ 0x05E3C000
    # b ~ 0x11BC0000
    # c ~ 0x00001F00
    # d ~ 0x00030074
    
    a = random.randint(0x01000000, 0x07000000) & 0xFFFFFF00
    b = random.randint(0x08000000, 0x14000000) & 0xFFFFFF00
    c = random.randint(0x00001000, 0x0000FFFF) & 0xFFFFFF00
    
    # d is the remainder
    d = (target_sig - (a + b + c)) & 0xFFFFFFFF
    
    header_path = os.path.join(src_dir, 'build_config.h')
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
    src_path = 'src'
    if len(sys.argv) > 1:
        src_path = sys.argv[1]
    generate_header(src_path)
