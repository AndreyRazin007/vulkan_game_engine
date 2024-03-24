#pragma once

#include "defines.h"

typedef enum rendererBackendType {
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX
} rendererBackendType;

typedef struct rendererBackend {
    struct platformState *platformState;
    u64 frameNumber;

    b8 (*initialize)(struct rendererBackend *backend,
                     const char *applicationName,
                     struct platformState *platformState);

    void (*shutdown)(struct rendererBackend *backend);

    void (*resized)(struct rendererBackend *backend, u16 width, u16 height);

    b8 (*beginFrame)(struct rendererBackend *backend, f32 deltaTime);
    b8 (*endFrame)(struct rendererBackend *backend, f32 deltaTime);    
} rendererBackend;

typedef struct renderPacket {
    f32 deltaTime;
} renderPacket;
