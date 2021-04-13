cmake_minimum_required(VERSION 3.12)

find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

project(${PROJECT_NAME}_hello-triangle)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm glslang::HLSL glslang::SPIRV glslang::glslang glslang::OGLCompiler Vulkan-Helper)
