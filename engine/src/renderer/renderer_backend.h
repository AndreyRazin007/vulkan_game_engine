#pragma once

#include "renderer_types.inl"

struct platformState;

b8 rendererBackendCreate(rendererBackendType type,
                         struct platformState *platformState,
                         rendererBackend *outRendererBackend);
void rendererBackendDestroy(rendererBackend *rendererBackend);
