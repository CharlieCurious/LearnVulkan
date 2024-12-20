#ifndef GLFW_INCLUDE_VULKAN
    #define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#endif
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <validation_layers.h>
#include <vk_instance.h>

static bool queueFamilyIndiciesIsComplete(struct QueueFamilyIndicies inicies);
static struct QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
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
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = getValidationLayers();

        vkDebugMessengerCreateInfo_Populate(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    if (vkCreateInstance(&createInfo, NULL, &app->instance) != VK_SUCCESS) {
        THROW("Failed to create VkInstance");
    }
}

void app_SetupDebugMessenger(App *app) {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    vkDebugMessengerCreateInfo_Populate(&createInfo);

    if (vkDebugUtilsMessengerEXT_Create(app->instance, &createInfo, NULL, &app->debugMessenger) != VK_SUCCESS) {
        THROW("Failed to create DebugUtilsMessengerEXT");
    }
}

void app_CreateSurface(App *app) {
    if (glfwCreateWindowSurface(app->instance, app->window, NULL, &app->surface) != VK_SUCCESS) {
        THROW("Failed to create window surface");
    }
}

void app_PickPhysicalDevice(App *app) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        THROW("Failed to find GPUs with Vulkan support!");
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, devices);
    
    for (uint32_t i = 0; i < deviceCount; i++) {
        if (isDeviceSuitable(devices[i], app->surface)) {
            app->physicalDevice = devices[i];
            break;
        }
    }
    
    if (app->physicalDevice == VK_NULL_HANDLE) {
        THROW("Failed to find a suitable GPU");
    }

}

void app_CreateLogicalDevice(App *app) {
    struct QueueFamilyIndicies indicies = findQueueFamilies(app->physicalDevice, app->surface);

    const uint32_t uniqueQueueFamiliesCount = (indicies.graphicsFamily.value == indicies.presentFamily.value) ? 1 : 2;
    uint32_t uniqueQueueFamilies[2] = {0};

    uniqueQueueFamilies[0] = indicies.graphicsFamily.value;
    if (uniqueQueueFamiliesCount == 2) {
        uniqueQueueFamilies[1] = indicies.presentFamily.value;
    }

    VkDeviceQueueCreateInfo queueCreateInfos[uniqueQueueFamiliesCount];

    float queuePriority = 1.0f;
    for (uint32_t i = 0; i < uniqueQueueFamiliesCount; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = uniqueQueueFamiliesCount;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = getValidationLayers();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(app->physicalDevice, &createInfo, NULL, &app->device) != VK_SUCCESS) {
        THROW("failed to create logical device!");
    }

    vkGetDeviceQueue(app->device, indicies.graphicsFamily.value, 0, &app->graphicsQueue);
    vkGetDeviceQueue(app->device, indicies.presentFamily.value, 0, &app->presentQueue);
}

// --------------------- Static Definitions ---------------------------------------------------------- //

static bool queueFamilyIndiciesIsComplete(struct QueueFamilyIndicies indicies) {
    return indicies.graphicsFamily.hasValue && indicies.presentFamily.hasValue;
}

static struct QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    struct QueueFamilyIndicies indicies = {0};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indicies.graphicsFamily.value = i;
            indicies.graphicsFamily.hasValue = true;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indicies.presentFamily.value = i;
            indicies.presentFamily.hasValue = true;
        }

        if (queueFamilyIndiciesIsComplete(indicies))
            break;

    }

    return indicies;
}

static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    struct QueueFamilyIndicies indicies = findQueueFamilies(device, surface);

    return queueFamilyIndiciesIsComplete(indicies);
}

static const char **getRequiredExtensions(uint32_t *extensionCount) {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    *extensionCount = glfwExtensionCount;
    const char **extensions = (const char **)malloc(glfwExtensionCount * sizeof(const char *));
    if (!extensions) {
        THROW("malloc fail: getRequiredExtensions");
    }

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions[i] = glfwExtensions[i];
    }

    if (enableValidationLayers) {
        const char **new_extensions = (const char **)realloc(extensions, (glfwExtensionCount + 1) *sizeof(const char *));
        if (!new_extensions) {
            free(extensions);
            THROW("malloc fail: getRequiredExtensions");
        }
        extensions = new_extensions;
        extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        (*extensionCount)++;
    }

return extensions;
}
