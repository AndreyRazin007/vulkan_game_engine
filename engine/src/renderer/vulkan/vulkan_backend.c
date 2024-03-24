#include "vulkan_backend.h"

#include "vulkan_types.inl"

#include "core/logger.h"

/* static Vulkan context. */
static vulkanContext context;

b8 vulkanRendererBackendInitialize(rendererBackend *backend,
                                   const char* applicationName,
                                   struct platformState* platformState) {
    /* custom allocator. */
    context.allocator = 0;

    /* Setup Vulkan instance. */
    VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = applicationName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Freak Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&createInfo, context.allocator, &context.instance);
    if(result != VK_SUCCESS) {
        FERROR("vkCreateInstance failed with result: %u", result);

        return false;
    }

    FINFO("Vulkan renderer initialized successfully.");

    return true;
}

void vulkanRendererBackendShutdown(rendererBackend *backend) {}

void vulkanRendererBackendOnResized(rendererBackend *backend,
                                    u16 width, u16 height) {}

b8 vulkanRendererBackendBeginFrame(rendererBackend *backend, f32 deltaTime) {
    return true;
}

b8 vulkanRendererBackendEndFrame(rendererBackend *backend, f32 deltaTime) {
    return true;
}
