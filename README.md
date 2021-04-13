# Vulkan-Helper
A set of classes and functions intended on easing general development with Vulkan

In order to use this library, at the moment one needs to have the following dependencies:

 - vcpkg installed
    - cmake directed to vcpkg by passing the command line argument `-DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake` at configure time
    - installed `glm`, `fmt` and `glslang` through vcpkg
