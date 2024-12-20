#include <stdio.h>
#include <stdlib.h>
#include <validation_layers.h>
#include <vk_instance.h>
#include <vulkan/vulkan_core.h>

static bool queueFamilyIndiciesIsComplete(struct QueueFamilyIndicies inicies);
static struct QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device);
static bool isDeviceSuitable(VkPhysicalDevice device);
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
        perror("Failed to create VkInstance");
        exit(EXIT_FAILURE);
    }
}

void app_SetupDebugMessenger(App *app) {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    vkDebugMessengerCreateInfo_Populate(&createInfo);

    if (vkDebugUtilsMessengerEXT_Create(app->instance, &createInfo, NULL, &app->debugMessenger) != VK_SUCCESS) {
        perror("Failed to create DebugUtilsMessengerEXT");
        exit(EXIT_FAILURE);
    }
}

void app_PickPhysicalDevice(App *app) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        perror("Failed to find GPUs with Vulkan support!");
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, devices);
    
    for (uint32_t i = 0; i < deviceCount; i++) {
        if (isDeviceSuitable(devices[i])) {
            app->physicalDevice = devices[i];
            break;
        }
    }
    
    if (app->physicalDevice == VK_NULL_HANDLE) {
        perror("Failed to find a suitable GPU");
        exit(EXIT_FAILURE);
    }

}

void app_CreateLogicalDevice(App *app) {
    struct QueueFamilyIndicies indicies = findQueueFamilies(app->physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indicies.graphicsFamily.value;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = getValidationLayers();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(app->physicalDevice, &createInfo, NULL, &app->device) != VK_SUCCESS) {
        perror("failed to create logical device!");
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(app->device, indicies.graphicsFamily.value, 0, &app->graphicsQueue);
}

// --------------------- Static Definitions ---------------------------------------------------------- //

static bool queueFamilyIndiciesIsComplete(struct QueueFamilyIndicies indicies) {
    return indicies.graphicsFamily.hasValue;
}

static struct QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device) {
    struct QueueFamilyIndicies indicies = {0};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    int j = 0;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indicies.graphicsFamily.value = j;
            indicies.graphicsFamily.hasValue = true;
        }

        if (queueFamilyIndiciesIsComplete(indicies))
            break;

        j++;
    }

    return indicies;
}

static bool isDeviceSuitable(VkPhysicalDevice device) {
    struct QueueFamilyIndicies indicies = findQueueFamilies(device);

    return queueFamilyIndiciesIsComplete(indicies);
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

    if (enableValidationLayers) {
        const char **new_extensions = (const char **)realloc(extensions, (glfwExtensionCount + 1) *sizeof(const char *));
        if (!new_extensions) {
            perror("malloc fail: getRequiredExtensions");
            free(extensions);
            exit(EXIT_FAILURE);
        }
        extensions = new_extensions;
        extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        (*extensionCount)++;
    }

return extensions;
}
