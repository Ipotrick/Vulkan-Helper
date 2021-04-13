cmake_minimum_required(VERSION 3.12)

project(${PROJECT_NAME}_samples)

add_subdirectory(compute)

option(VULKAN_HELPER_SAMPLES_WIN32 "Turn on to include win32 samples" OFF)
option(VULKAN_HELPER_SAMPLES_GLFW "Turn on to include glfw samples" OFF)

if (VULKAN_HELPER_SAMPLES_WIN32)
    add_subdirectory(win32)
endif()

if (VULKAN_HELPER_SAMPLES_GLFW)
    add_subdirectory(glfw)
endif()
