# Vulkan-Helper
A set of classes and functions intended on easing general development with Vulkan

## Building with 
Vulkan Helper will hopefully have vcpkg integration, but for the moment, one just needs to have the header file, as it is a header only library. 
Vulkan-Helper requires one to have The VulkanSDK installed on their system, which means having it added to the PATH (this is done automatically on Windows when running the installer)
Just include the header into any C++ source file, making sure to do so with the following macro definition in exactly ONE file:
```cpp
#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>
```

## Building the samples
The samples require additional dependencies on top of just the VulkanSDK.
So far, all the samples require `glslang` `glm` and `fmt` to be installed through vcpkg, and the GLFW samples require one installing `glfw3`
