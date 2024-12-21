#include <shaders.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <vulkan/vulkan_core.h>

uint32_t *readShaderSource(char *fileName, size_t *outSize) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        THROW("Failed to open shader file");
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        THROW("Failed to seek the end of shader source file");
    }

    long fileSize = ftell(file);
    if (fileSize <= 0 || fileSize % 4 != 0) {
        fclose(file);
        THROW("Invalid SPIR-V file size");
    }

    rewind(file);
    
    uint32_t *buffer = (uint32_t *)malloc(fileSize);
    if (!buffer) {
        fclose(file);
        THROW("malloc fail in readShaderSource");
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != (size_t)fileSize) {
        free(buffer);
        fclose(file);
        THROW("Failed to read shader source file");
    }

    fclose(file);
    
    if (outSize) {
        *outSize = (size_t)fileSize;
    }

    return buffer;
}

VkShaderModule createShaderModule(VkDevice device, const uint32_t *code, size_t size) {
    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = code;

    VkShaderModule shaderModule = {0};
    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        THROW("Failed to create shader module!");
    }

    return shaderModule;
}
