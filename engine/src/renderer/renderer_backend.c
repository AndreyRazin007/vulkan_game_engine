#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 rendererBackendCreate(rendererBackendType type,
                         struct platformState *platformState,
                         rendererBackend *outRendererBackend) {
    outRendererBackend->platformState = platformState;

    if (type == RENDERER_BACKEND_TYPE_VULKAN) {
        outRendererBackend->initialize = vulkanRendererBackendInitialize;
        outRendererBackend->shutdown = vulkanRendererBackendShutdown;
        outRendererBackend->beginFrame = vulkanRendererBackendBeginFrame;
        outRendererBackend->endFrame = vulkanRendererBackendEndFrame;
        outRendererBackend->resized = vulkanRendererBackendOnResized;

        return true;
    }

    return false;
}

void rendererBackendDestroy(rendererBackend *rendererBackend) {
    rendererBackend->initialize = 0;
    rendererBackend->shutdown = 0;
    rendererBackend->beginFrame = 0;
    rendererBackend->endFrame = 0;
    rendererBackend->resized = 0;
}
