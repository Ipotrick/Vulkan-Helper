cmake_minimum_required(VERSION 3.12)

find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

project(${PROJECT_NAME}_resizable-window)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt glm::glm glslang::HLSL glslang::SPIRV glslang::glslang glslang::OGLCompiler Vulkan-Helper)
# This would be done automatically if you linked against the vcpkg version
target_include_directories(${PROJECT_NAME} PRIVATE "../../../include")
