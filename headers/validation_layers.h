#pragma once

#include <stdbool.h>
#include <vulkan/vulkan_core.h>

extern const bool enableValidationLayers;

#define VALIDATION_LAYERS_COUNT 1

const char **getValidationLayers();

void vkDebugMessengerCreateInfo_Populate(VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo);

VkResult vkDebugUtilsMessengerEXT_Create(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger);

void vkDebugUtilsMessengerEXT_Destroy(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator);

