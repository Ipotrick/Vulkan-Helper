cmake_minimum_required(VERSION 3.12)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(
REQUIRES
    glm/0.9.9.5
    glslang/8.13.3559
OPTIONS
    BASIC_SETUP
    CMAKE_TARGETS
BUILD
    missing
)

project(${PROJECT_NAME}_hello-triangle)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan-Helper CONAN_PKG::glm CONAN_PKG::glslang)
