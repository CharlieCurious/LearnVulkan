#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#define VALIDATION_LAYERS_COUNT 1
const char *validationLayers[VALIDATION_LAYERS_COUNT] = { "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

VkResult createDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger) {

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(
        VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks *pAllocator) {

    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

typedef enum APP_Result {
    APP_SUCCESS,
    APP_ERROR,
} APP_Result;

typedef struct AppHelloTriangle {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
} AppHelloTriangle;

bool checkValidationSupport() {
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties availableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (size_t i = 0; i < VALIDATION_LAYERS_COUNT; i++) {
        const char *layerName = validationLayers[i];
        bool layerFound = false;

        for (uint32_t j = 0; j < layerCount; j++) {
            if (strcmp(layerName, availableLayers[j].layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        
        if (!layerFound) 
            return false;
    }

    return true;
}

const char **getRequiredExtensions(uint32_t *extensionCount) {
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
        const char **new_extensions = (const char **)realloc(extensions, (glfwExtensionCount + 1) * sizeof(const char *));
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

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagBitsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {

    fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);
    
   return VK_FALSE; 
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
    (*createInfo).sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    (*createInfo).messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    (*createInfo).messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    (*createInfo).pfnUserCallback = debugCallback;
    (*createInfo).pUserData = NULL;
    (*createInfo).pNext = NULL;
    (*createInfo).flags = 0;
}

void app_SetupDebugMessenger(AppHelloTriangle *app) {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);

    if (createDebugUtilsMessengerEXT(app->instance, &createInfo, NULL, &app->debugMessenger) != VK_SUCCESS) {
        perror("Failed to create DebugUtilsMessengerEXT");
        exit(EXIT_FAILURE);
    }
}

void app_InitWindow(AppHelloTriangle *app) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    app->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

void app_CreateVkInstance(AppHelloTriangle *app) {
    if (enableValidationLayers && !checkValidationSupport()) {
        perror("Validation layers requested, but not available.");
        exit(EXIT_FAILURE);
    }

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

    // Optional: check all extensions
    // ---------------------------------------------------------------
    //VkResult extensionEnumResult;
    //uint32_t extensionCount = 0;
    //extensionEnumResult = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    //if (extensionEnumResult != VK_SUCCESS)
    //    perror("Failed to count extensions");

    //VkExtensionProperties extensions[extensionCount];
    //extensionEnumResult = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

    //if (extensionEnumResult != VK_SUCCESS)
    //    perror("Failed to enumerate extensions");

    //puts("Available Extensions:");
    //for (uint32_t i = 0; i < extensionCount; i++) {
    //    printf("\t%s\n", extensions[i].extensionName);
    //}


    //printf("Found %d extensions\n", extensionCount); 
    //-----------------------------------------------------------------

    uint32_t extensionCount = 0;
    const char **extensions;

    extensions = getRequiredExtensions(&extensionCount);

    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    VkResult result = vkCreateInstance(&createInfo, NULL, &app->instance);

    if (result != VK_SUCCESS) {
        perror("Failed to create VkInstance");
        exit(EXIT_FAILURE);
    }
}

bool isDeviceSuitable(VkPhysicalDevice device) {
    return true;
}

void app_PickPhysicalDevide(AppHelloTriangle *app) {
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

void app_InitVulkan(AppHelloTriangle *app) {
    app_CreateVkInstance(app);
    app_SetupDebugMessenger(app);
    app_PickPhysicalDevide(app);
}

void app_MainLoop(AppHelloTriangle *app) {
    while(!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();
    }
}

void app_Cleanup(AppHelloTriangle *app) {
    if (!app)
        return;

    if (app->instance) {
        if (enableValidationLayers) {
            destroyDebugUtilsMessengerEXT(app->instance, app->debugMessenger, NULL);
        }
        vkDestroyInstance(app->instance, NULL);
    }

    if (app->window) {
        glfwDestroyWindow(app->window);
        glfwTerminate();
    }

}

void app_Run(AppHelloTriangle *app, APP_Result *result) {
    *result = APP_ERROR;

    app_InitWindow(app);
    app_InitVulkan(app);
    app_MainLoop(app);
    app_Cleanup(app);

    *result = APP_SUCCESS;
}

int main() {
    AppHelloTriangle app = {0};
    
    APP_Result result;
    app_Run(&app, &result);
    if (result != APP_SUCCESS) {
        perror ("APP_Result is Failure");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
