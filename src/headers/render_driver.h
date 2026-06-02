#ifndef RENDER_DRIVER_H
#define RENDER_DRIVER_H

#include <stdint.h>
#include <wchar.h>

typedef struct RenderDriver {
    void (*set_color)(struct RenderDriver *self, int r, int g, int b);
    void (*reset_color)(struct RenderDriver *self);
    void (*put_char)(struct RenderDriver *self, wchar_t c);
    void *ctx;
} RenderDriver;

#endif
