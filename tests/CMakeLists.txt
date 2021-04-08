cmake_minimum_required(VERSION 3.12)

project(${PROJECT_NAME}_tests)

add_executable(${PROJECT_NAME}_main main.cpp)
target_link_libraries(${PROJECT_NAME}_main PRIVATE Vulkan-Helper)
