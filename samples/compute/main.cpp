#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

int main() {
	auto vulkanInstance = vkh::createInstance({}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME});
	auto debugMessenger = vkh::createDebugMessenger(vulkanInstance);

	std::vector<const char *> deviceExtensions{};
	std::size_t computeQueueIndex = 0;

	auto selectedPhysicalDevice = vkh::selectPhysicalDevice(vulkanInstance, [&](vk::PhysicalDevice device) -> std::size_t {
		std::size_t score = 0;
		return score;
	});

	auto selectedPhysicalDeviceProperties = selectedPhysicalDevice.getProperties();
	fmt::print("Selected Physical Device: {}\n", selectedPhysicalDeviceProperties.deviceName);

	auto logicalDevice = vkh::createLogicalDevice(selectedPhysicalDevice, {computeQueueIndex}, deviceExtensions);
}
