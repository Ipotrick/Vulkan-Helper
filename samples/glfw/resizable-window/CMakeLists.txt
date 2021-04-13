cmake_minimum_required(VERSION 3.12)

find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

project(${PROJECT_NAME}_resizable-window)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan-Helper glm::glm glslang::HLSL glslang::SPIRV glslang::glslang glslang::OGLCompiler glfw)
