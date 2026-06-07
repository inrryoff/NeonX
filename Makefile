# * NeonX — Terminal Shader Engine
# * Copyright (C) 2026  inrryoff
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program. If not, see <https://www.gnu.org/licenses/>.

VERSION = "2.2.2-STABLE"
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
       $(SRC_DIR)/render_driver.h \
       $(SRC_DIR)/build_config.h

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

all: release

GEN_CONFIG = tools/gen_config
$(GEN_CONFIG): tools/gen_config.c
	@clang -O2 tools/gen_config.c -o $(GEN_CONFIG)

$(SRC_DIR)/build_config.h: $(GEN_CONFIG) $(SRCS)
	@echo "Generating build configuration..."
	@$(GEN_CONFIG) $(SRC_DIR) $(SRC_DIR)/build_config.h

release: $(SRC_DIR)/build_config.h $(TARGET)

debug: CFLAGS = -g -O0 -DDEBUG -Isrc -DVERSION=\"$(VERSION)\" -DBUILD_STATUS=\"$(BUILD_STATUS)\" -DBUILD_MAINTAINER=\"$(BUILD_MAINTAINER)\"
debug: $(SRC_DIR)/build_config.h $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

wasm:
	@mkdir -p $(BIN_DIR)
	emcc -O2 -s WASM=1 \
		-s EXPORTED_FUNCTIONS='["_neonx_wasm_init", "_neonx_apply_colors", "_neonx_wasm_set_frequency", "_neonx_wasm_set_opacity", "_neonx_wasm_set_quantization", "_neonx_wasm_set_custom_gradient", "_neonx_wasm_reset_palette", "_neonx_wasm_set_palette_offsets", "_neonx_wasm_set_preset", "_malloc", "_free"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall", "UTF8ToString"]' \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s NO_EXIT_RUNTIME=1 \
		-Isrc \
		src/math_fixed.c src/shader_effects.c src/render_core.c \
		src/main_wasm.c src/msgs.c src/shaders.c \
		-o $(BIN_DIR)/neonx.js

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(GEN_CONFIG)
