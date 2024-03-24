#pragma once

#include "renderer/renderer_backend.h"

b8 vulkanRendererBackendInitialize(rendererBackend* backend,
                                      const char* application_name,
                                      struct platformState* platformState);
void vulkanRendererBackendShutdown(rendererBackend* backend);

void vulkanRendererBackendOnResized(rendererBackend* backend,
                                        u16 width, u16 height);

b8 vulkanRendererBackendBeginFrame(rendererBackend* backend, f32 delta_time);
b8 vulkanRendererBackendEndFrame(rendererBackend* backend, f32 delta_time);
