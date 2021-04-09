#include <iostream>

#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

#include "window.hpp"

int main() {
	vkh::Context vkContext;

	Window window;
	vkContext.createSurface(window.vulkanSurfaceCreateInfo());

	auto physicalDevice = vkContext.selectPhysicalDevice([]() {
		size_t score = 0;
		return score;
	});

	auto logicalDevice = vkContext.createLogicalDevice();
}
