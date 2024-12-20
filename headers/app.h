#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct App {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    VkImage *swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
} App;

typedef enum APP_Result {
    APP_ERROR,
    APP_SUCCESS,
} APP_Result;

void app_Run(App *app, APP_Result *result);
void app_InitWindow(App *app);
void app_InitVulkan(App *app);
void app_CreateSwapChain(App *app);
void app_MainLoop(App *app);
void app_Cleanup(App *app);
