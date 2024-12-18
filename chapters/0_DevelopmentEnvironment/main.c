#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

#include <stdio.h>


int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", NULL, NULL);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    fprintf(stderr, "%d extensions supported\n", extensionCount);

    mat4 matrix;
    glm_mat4_identity(matrix);
    vec4 vec = { 1.0f, 2.0f, 3.0f, 1.0f };

    vec4 result;
    glm_mat4_mulv(matrix, vec, result);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
