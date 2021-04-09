cmake_minimum_required(VERSION 3.12)

find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE Vulkan::Vulkan)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
