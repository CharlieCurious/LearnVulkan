#include <app.h>
#include <shaders.h>
#include <swapchain.h>
#include <validation_layers.h>
#include <vk_instance.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

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
    app_CreateSurface(app);
    app_PickPhysicalDevice(app);
    app_CreateLogicalDevice(app);
    app_CreateSwapChain(app);
    app_CreateImageViews(app);
    app_CreateGraphicsPipeline(app);
}

void app_CreateSwapChain(App *app) {
    uint32_t formatCount;
    uint32_t presentModeCount;
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(app->physicalDevice, app->surface, &formatCount, &presentModeCount);
    
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, presentModeCount);
    VkExtent2D extent = chooseSwapExtent(&swapChainSupport.capabilities, app->window);

    // Simply sticking to this minimum means that we may sometimes have to wait on the driver to complete internal operations before we can acquire another image to render to. 
    // Therefore it is recommended to request at least one more image than the minimum.
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = app->surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    struct QueueFamilyIndicies indicies = findQueueFamilies(app->physicalDevice, app->surface);
    uint32_t queueFamilyIndicies[] = {indicies.graphicsFamily.value, indicies.presentFamily.value };
    
    if (indicies.graphicsFamily.value != indicies.presentFamily.value) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndicies;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = NULL; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(app->device, &createInfo, NULL, &app->swapChain) != VK_SUCCESS) {
        THROW("Failed to create swap chain");
    }
    
    vkGetSwapchainImagesKHR(app->device, app->swapChain, &imageCount, NULL);
    app->pSwapChainImages = (VkImage *)malloc(imageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(app->device, app->swapChain, &imageCount, app->pSwapChainImages);
    app->swapChainImageCount = imageCount;

    app->swapChainImageFormat = surfaceFormat.format;
    app->swapChainExtent = extent;
}

void app_CreateImageViews(App *app) {
    app->pSwapChainImageViews = (VkImageView *)malloc(app->swapChainImageCount * sizeof(VkImageView));

    for (uint32_t i = 0; i < app->swapChainImageCount; i++) {
        VkImageViewCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = app->pSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = app->swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(app->device, &createInfo, NULL, &app->pSwapChainImageViews[i]) != VK_SUCCESS) {
            THROW("Failed to create image views!");
        }
    }
}

void app_CreateGraphicsPipeline(App *app) {
    size_t vertShaderSize, fragShaderSize;
    uint32_t *pVertShader = readShaderSource("shaders/bin/vert.spv", &vertShaderSize);
    uint32_t *pFragShader = readShaderSource("shaders/bin/frag.spv", &fragShaderSize);

    VkShaderModule vertShaderModule = createShaderModule(app->device, pVertShader, vertShaderSize);
    VkShaderModule fragShaderModule = createShaderModule(app->device, pFragShader, fragShaderSize);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[2] = { vertShaderStageInfo, fragShaderStageInfo };

    vkDestroyShaderModule(app->device, vertShaderModule, NULL);
    vkDestroyShaderModule(app->device, fragShaderModule, NULL);
}

void app_MainLoop(App *app) {
    while (!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();
    }
}

void app_Cleanup(App *app) {
    if (!app)
        return;

    if (app->pSwapChainImageViews) {
        for (uint32_t i = 0; i < app->swapChainImageCount; i++) {
            vkDestroyImageView(app->device, app->pSwapChainImageViews[i], NULL);
        }
        free(app->pSwapChainImageViews);
    }

    if (app->device) {
        if (app->swapChain) {
            vkDestroySwapchainKHR(app->device, app->swapChain, NULL);
        }
        vkDestroyDevice(app->device, NULL);
    }

    if (app->pSwapChainImages) {
        free(app->pSwapChainImages);
    }
    

    if (app->instance) {
        if (enableValidationLayers) {
           vkDebugUtilsMessengerEXT_Destroy(app->instance, app->debugMessenger, NULL); 
        }
        if (app->surface) {
            vkDestroySurfaceKHR(app->instance, app->surface, NULL);
        }
        vkDestroyInstance(app->instance, NULL);
    }

    if (app->window) {
        glfwDestroyWindow(app->window);
        glfwTerminate();
    }
}
