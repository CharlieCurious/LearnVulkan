#include "vk_instance.h"

#include <stdio.h>
#include <stdlib.h>

static const char **getRequiredExtensions(uint32_t *extensionCount);

void app_CreateVkInstance(App *app) {
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t extensionCount = 0;
    const char **extensions = getRequiredExtensions(&extensionCount);
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;

    // Validation Layers
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = NULL;

    if (vkCreateInstance(&createInfo, NULL, &app->instance) != VK_SUCCESS) {
        perror("Failed to create VkInstance");
        exit(EXIT_FAILURE);
    }
}

void app_SetupDebugMessenger(App *app) {
}

void app_PickPhysicalDevice(App *app) {
}

void app_CreateLogicalDevice(App *app) {
}


static const char **getRequiredExtensions(uint32_t *extensionCount) {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    *extensionCount = glfwExtensionCount;
    const char **extensions = (const char **)malloc(glfwExtensionCount * sizeof(const char *));
    if (!extensions) {
        perror("malloc fail: getRequiredExtensions");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions[i] = glfwExtensions[i];
    }

    return extensions;
}
