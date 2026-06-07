/*
 * NeonX — Terminal Shader Engine
 * Copyright (C) 2026  inrryoff
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MSGS_H
#define MSGS_H

#include <stdbool.h>
#include <stdint.h>

uint32_t voffset(void);

enum Mensagem {
    MSG_ERRO_ABRIR,

    MSG_VERSION_NX_BUILD_CTX_ID,
    MSG_VERSION_COMPILED_BY,
    MSG_VERSION_STATUS_OFFICIAL,
    MSG_VERSION_STATUS_VERIFIED_BY,
    MSG_VERSION_STATUS_MODIFIED,
    MSG_VERSION_STATUS_ERROR,

    MSG_LICENSE_TEXT,

    MSG_HELP_HEADER,
    MSG_HELP_TEXT,

    MSG_ERR_MISSING_VALUE,
    MSG_ERR_INVALID_NUMBER,
    MSG_ERR_INVALID_INTEGER,
    MSG_ERR_MODE_LIMIT,
    MSG_ERR_INVALID_OPTION,
    MSG_ERR_NO_DATA,
    MSG_ERR_FILE_TOO_LARGE,
    MSG_ERR_INTEGRITY_FAIL,
    MSG_ERR_INTEGRITY_OPEN,
    MSG_ERR_INTEGRITY_READ,
    MSG_ERR_INTEGRITY_MEM,
    MSG_ERR_INTEGRITY_SIGNATURE,
    MSG_ERR_INTEGRITY_SIZE,
    MSG_ERR_MUST_BE_INTEGER,
    MSG_ERR_MUST_BE_POSITIVE,
    MSG_ERR_DURATION_NEGATIVE,
    MSG_ERR_MEMORY_ALLOC,
    MSG_ERR_BUFFER_TRUNCATED,
    MSG_ERR_CUSTOM_KEY_FAIL,
    MSG_DEBUG_ALIGNMENT,
    MSG_VERIFY_OK,
    MSG_VERIFY_FAIL,

    MSG_TOTAL
};

const char* get_msg(enum Mensagem id);
#define MSG(id) get_msg(id)
const char* MSG_F(enum Mensagem id, bool disable_ansi);


#ifdef _WIN32
const char* msgs_detect_windows_locale(void);
#endif

void msgs_init(void);
void msgs_set_language(const char *lang_code);

#endif
