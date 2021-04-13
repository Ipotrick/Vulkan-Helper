cmake_minimum_required(VERSION 3.12)

project(${PROJECT_NAME}_samples)

add_subdirectory(compute)

# add_subdirectory("glfw/hello-triangle")
# add_subdirectory("glfw/resizable-window")

add_subdirectory("win32/hello-triangle")
add_subdirectory("win32/resizable-window")
