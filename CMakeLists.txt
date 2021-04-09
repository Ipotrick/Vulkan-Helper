cmake_minimum_required(VERSION 3.12)

project(Vulkan-Helper)
add_subdirectory(src)

option(buildTests "Turn on to build tests" ON)
option(buildSamples "Turn on to build samples" ON)

if (buildTests)
    add_subdirectory(tests)
endif()
if (buildSamples)
    add_subdirectory(samples)
endif()
