cmake_minimum_required(VERSION 3.12)

find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

project(${PROJECT_NAME}_compute)

set(SPV_REFL "$ENV{VULKAN_SDK}/Source/SPIRV-Reflect/spirv_reflect.c")
string(REPLACE "\\" "/" SPV_REFL "${SPV_REFL}")

add_executable(${PROJECT_NAME} main.cpp ${SPV_REFL})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt glm::glm glslang::SPIRV Vulkan-Helper)
# This would be done automatically if one linked against the vcpkg version
target_include_directories(${PROJECT_NAME} PRIVATE "../../include")

target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/Source/SPIRV-Reflect")
target_compile_definitions(${PROJECT_NAME} PRIVATE "VULKANHELPER_SPIRV_REFLECT_INCLUDE_PATH=<spirv_reflect.h>")
