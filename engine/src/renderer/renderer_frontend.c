#include "renderer_frontend.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "core/memory.h"

struct platformState;

// Backend render context.
static rendererBackend* backend = 0;

b8 rendererInitialize(const char *applicationName,
                      struct platformState *platformState) {
    backend = allocate(sizeof(rendererBackend), MEMORY_TAG_RENDERER);

    /* make this configurable. */
    rendererBackendCreate(RENDERER_BACKEND_TYPE_VULKAN, platformState, backend);
    backend->frameNumber = 0;

    if (!backend->initialize(backend, applicationName, platformState)) {
        FFATAL("Renderer backend failed to initialize. Shutting down.");

        return false;
    }

    return true;
}

void rendererShutdown() {
    backend->shutdown(backend);
    free(backend, sizeof(rendererBackend), MEMORY_TAG_RENDERER);
}

b8 rendererBeginFrame(f32 deltaTime) {
    return backend->beginFrame(backend, deltaTime);
}

b8 rendererEndFrame(f32 deltaTime) {
    b8 result = backend->endFrame(backend, deltaTime);
    ++backend->frameNumber;

    return result;
}

b8 rendererDrawFrame(renderPacket *packet) {
    /* If the begin frame returned successfully, mid-frame operations may continue. */
    if (rendererBeginFrame(packet->deltaTime)) {
        /* End the frame. If this fails, it is likely unrecoverable. */
        b8 result = rendererEndFrame(packet->deltaTime);

        if (!result) {
            FERROR("renderer_end_frame failed. Application shutting down...");

            return false;
        }
    }

    return true;
}
