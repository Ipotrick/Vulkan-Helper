cmake_minimum_required(VERSION 3.12)

project(${PROJECT_NAME}_compute)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan-Helper)
