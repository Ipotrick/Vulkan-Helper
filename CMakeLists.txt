cmake_minimum_required(VERSION 3.12)

project(Vulkan-Helper)

add_subdirectory(src)

option(buildTests "Turn on to build tests" ON)

if (buildTests)
    add_subdirectory(tests)
endif()
