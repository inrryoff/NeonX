# NeonX Architecture

This document describes the internal structure and design principles of the **NeonX** rendering engine.

## Core Design Principles

1.  **Fixed-Point Arithmetic (16.16)**: To ensure consistent performance and visual results across different platforms (including those without an FPU, like some embedded systems or specific WASM environments), NeonX uses fixed-point math for all shader calculations.
2.  **Driver-Based Rendering**: The rendering logic is decoupled from the output method. A `RenderDriver` structure defines how to set colors and print characters, allowing the same core to render to a terminal, a WASM buffer, or a GUI.
3.  **TTY Intelligence**:
    * **Input Protection**: Prevents the program from hanging when run interactively without piped data. It detects `STDIN` as a TTY and prompts for usage.
    * **Output Sanitization**: Detects if `STDOUT` is redirected to a file or pipe. In such cases, it automatically disables ANSI escape sequences (colors and terminal control codes), producing clean text output (e.g., for `neonx --license > LICENSE.txt`).
4.  **Performance Optimization**:
    * **Length Caching**: String lengths are cached in the `Content` structure to avoid redundant `wcslen` calls during animation.
    * **LUT (Look-Up Tables)**: Fast trigonometric functions use precomputed tables.
    * **Buffered Output**: Frames are built in memory and written to the terminal in a single operation to minimize syscall overhead and flickering.

## Project Structure

```text
.
├── core/                 # Core engine implementations (.c files)
│   ├── integrity.c       # Binary signing & verification logic
│   ├── math_fixed.c      # 16.16 Fixed-point math & LUTs
│   ├── monocypher.c      # Monocypher crypto engine implementation
│   ├── msgs.c            # I18n / Localized messages
│   ├── render.c          # High-level render loops (Buffered/Stream)
│   ├── render_core.c     # Core pixel-level color & line rendering
│   ├── shader_effects.c  # Low-level shader math (Sunset, Matrix, etc.)
│   ├── shaders.c         # Shader preset management
│   └── terminal.c        # Terminal state & memory management
├── headers/              # Interface definitions and API headers (.h files)
│   ├── build_config.h    # Dynamic build configurations & SyncID (Generated)
│   ├── integrity.h
│   ├── math_fixed.h
│   ├── math_fixed_internal.h
│   ├── monocypher.h
│   ├── msgs.h
│   ├── neonx.h           # Unified Public API Header
│   ├── render.h
│   ├── render_core.h
│   ├── render_driver.h   # Abstract Driver Interface
│   ├── shader_effects.h
│   ├── shaders.h
│   ├── style.h           # UI styling and ANSI logo macros
│   └── terminal.h
├── main.c                # CLI Entry point & Argument parsing
└── main_wasm.c           # Emscripten-compatible bridge interface for WASM

```
## Data Flow
 1. **Input**: load_input_data reads text from stdin, sanitizes it, and stores it in the Content structure, caching the length of each line.
 2. **Logic**: run_buffered_mode iterates through lines. For each frame, it updates the phase (animation state).
 3. **Core**: neonx_render_line is called for each line. It uses the RenderDriver to communicate with the output.
 4. **Shaders**: For each character (pixel), neonx_get_color calculates the RGB values based on coordinates, phase, and the active shader mode.
 5. **Output**: The RenderDriver implementation (e.g., in core/render.c for CLI) translates RGB values into ANSI escape sequences.
## Portability
 * **WASM**: main_wasm.c provides an Emscripten-compatible interface, exposing the core functions to JavaScript.
 * **Windows/Linux**: Platform-specific terminal handling (signals, VT sequences) is abstracted or handled via #ifdef.
## Security
 * **Integrity Check**: The binary includes an EdDSA signature at the end. core/integrity.c verifies this signature against a public key (official or generic) before execution to detect tampering.
```