#include "window.hpp"

#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

int main() {
    // instance creation
    // vk::Instance vulkanInstance;

	Window window({.frameSizeX = 800, .frameSizeY = 600, .title = "Hello Triangle"});
    // auto vulkanSurface = window.createVulkanSurface(vulkanInstance);

    // physical device selection
    // logical device creation
    // queue retrieval
    // swapchain creation

    // raw vulkan

    while (true) {
        window.handleEvents();
        if (!window.isOpen())
            break;
        
        // draw code
    }
}
