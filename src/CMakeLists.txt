cmake_minimum_required(VERSION 3.12)

find_package(Vulkan REQUIRED)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(
REQUIRES
    fmt/7.1.3
OPTIONS
    BASIC_SETUP
    CMAKE_TARGETS
BUILD
    missing
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE Vulkan::Vulkan CONAN_PKG::fmt)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
