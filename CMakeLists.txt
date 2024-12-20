cmake_minimum_required(VERSION 3.22.1)

project(LearnVulkan LANGUAGES C VERSION 1.0)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Needed for proper nvim lsp functioning
set(CMAKE_C_FLAGS "-Wall -Wextra -O2 -g")

set(EXE VulkanTest)
set(COMMON_LIBS glfw vulkan dl pthread X11 Xxf86vm Xrandr Xi)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB SRC "${SRC_DIR}/*.c")

add_executable(${EXE} ${SRC})
target_include_directories(${EXE} PRIVATE headers)
target_compile_options(${EXE} PRIVATE -Wall -Wextra -Wno-unused-parameter -O2 -g)
target_link_libraries(${EXE} PRIVATE ${COMMON_LIBS})
