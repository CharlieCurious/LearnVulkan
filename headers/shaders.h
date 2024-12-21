#pragma once

#include <app.h>
#include <stddef.h>
#include <vulkan/vulkan_core.h>

uint32_t *readShaderSource(char *fileName, size_t *outSize);

VkShaderModule createShaderModule(VkDevice device, const uint32_t *code, size_t size);
