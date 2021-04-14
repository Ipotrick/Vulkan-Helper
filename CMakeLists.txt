cmake_minimum_required(VERSION 3.12)

project(Vulkan-Helper)
add_subdirectory(src)

option(VULKAN_HELPER_BUILD_TESTS "Turn on to build tests" OFF)
option(VULKAN_HELPER_BUILD_SAMPLES "Turn on to build samples" OFF)

if (VULKAN_HELPER_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if (VULKAN_HELPER_BUILD_SAMPLES)   
    add_subdirectory(samples)
endif()
