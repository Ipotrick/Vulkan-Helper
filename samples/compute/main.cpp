#include <iostream>

#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

int main() {
	vkh::Context vkContext;

	auto physicalDevice = vkContext.selectPhysicalDevice([]() {
		size_t score = 0;
		return score;
	});

	auto logicalDevice = vkContext.createLogicalDevice();
}
