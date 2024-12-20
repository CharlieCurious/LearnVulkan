#include <app.h>
#include <validation_layers.h>
#include <vk_instance.h>

#include <stdint.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void app_Run(App *app, APP_Result *result) {
    *result = APP_ERROR;

    app_InitWindow(app);
    app_InitVulkan(app);
    app_MainLoop(app);
    app_Cleanup(app);

    *result = APP_SUCCESS;
}

void app_InitWindow(App *app) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    app->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

void app_InitVulkan(App *app) {
    app_CreateVkInstance(app);
    app_SetupDebugMessenger(app);
    app_PickPhysicalDevice(app);
    app_CreateLogicalDevice(app);
}

void app_MainLoop(App *app) {
    while (!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();
    }
}

void app_Cleanup(App *app) {
    if (!app)
        return;

    if (app->device) {
        vkDestroyDevice(app->device, NULL);
    }

    if (app->instance) {
        if (enableValidationLayers) {
           vkDebugUtilsMessengerEXT_Destroy(app->instance, app->debugMessenger, NULL); 
        }
        vkDestroyInstance(app->instance, NULL);
    }

    if (app->window) {
        glfwDestroyWindow(app->window);
        glfwTerminate();
    }
}
