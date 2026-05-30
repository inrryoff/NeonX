VERSION = "2.2.1-STABLE"
BUILD_STATUS = "STABLE_RELEASE"
BUILD_MAINTAINER = "COMMUNITY"

CC = clang
CFLAGS = -O3 -ffast-math -DNDEBUG -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wall -Wextra -Wconversion -Wsign-conversion -Wformat=2 -Wstrict-overflow=5 -Isrc \
         -DVERSION=\"$(VERSION)\" -DBUILD_STATUS=\"$(BUILD_STATUS)\" -DBUILD_MAINTAINER=\"$(BUILD_MAINTAINER)\"
LDFLAGS = -lm

# Detect OS
UNAME_S := $(shell uname -s)
UNAME_O := $(shell uname -o 2>/dev/null || echo Unknown)

ifeq ($(UNAME_S), Linux)
    ifeq ($(UNAME_O), Android)
        # Termux/Android
    else
        LDFLAGS += -Wl,-z,relro,-z,now -Wl,--as-needed
    endif
endif

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = build
TARGET = $(BIN_DIR)/neonx

# Headers for dependency tracking
HDRS = $(SRC_DIR)/integrity.h \
       $(SRC_DIR)/monocypher.h \
       $(SRC_DIR)/msgs.h \
       $(SRC_DIR)/render.h \
       $(SRC_DIR)/shaders.h \
       $(SRC_DIR)/terminal.h \
       $(SRC_DIR)/math_fixed.h \
       $(SRC_DIR)/shader_effects.h \
       $(SRC_DIR)/render_core.h \
       $(SRC_DIR)/render_driver.h

# Sources (excluding WASM main)
SRCS = $(SRC_DIR)/integrity.c \
       $(SRC_DIR)/main.c \
       $(SRC_DIR)/monocypher.c \
       $(SRC_DIR)/msgs.c \
       $(SRC_DIR)/render.c \
       $(SRC_DIR)/shaders.c \
       $(SRC_DIR)/terminal.c \
       $(SRC_DIR)/math_fixed.c \
       $(SRC_DIR)/shader_effects.c \
       $(SRC_DIR)/render_core.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean debug release wasm

all: release

release: $(TARGET)

debug: CFLAGS = -g -O0 -DDEBUG -Isrc -DVERSION=\"$(VERSION)\" -DBUILD_STATUS=\"$(BUILD_STATUS)\" -DBUILD_MAINTAINER=\"$(BUILD_MAINTAINER)\"
debug: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

wasm:
	@mkdir -p $(BIN_DIR)
	emcc -O2 -s WASM=1 \
		-s EXPORTED_FUNCTIONS='["_neonx_wasm_init", "_neonx_apply_colors", "_neonx_wasm_set_frequency", "_neonx_wasm_set_opacity", "_neonx_wasm_set_quantization", "_malloc", "_free"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall", "UTF8ToString"]' \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s NO_EXIT_RUNTIME=1 \
		-Isrc \
		src/math_fixed.c src/shader_effects.c src/render_core.c \
		src/main_wasm.c src/msgs.c \
		-o $(BIN_DIR)/neonx.js

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
