#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

std::size_t deviceRating(vk::PhysicalDevice) {
    return 0;
}

void testSuite() {
    std::vector<const char *> vectorCString = {"adsda", "asdasdwaa", "wadsdawdw"};
	vk::Instance inst[] = {
		vkh::createInstance({}, {}),
		vkh::createInstance({"layer"}, {}),
		vkh::createInstance({}, {"extension"}),
		vkh::createInstance({"layer"}, vectorCString),
		vkh::createInstance(vectorCString, {"extension", "e2"}),
	};

	int x = 3;
	std::size_t y = 4;

	vk::PhysicalDevice physicalDevices[] = {
		vkh::selectPhysicalDevice(inst[0], [&](vk::PhysicalDevice) -> std::size_t {
			return x;
		}),
		vkh::selectPhysicalDevice(inst[0], [&](vk::PhysicalDevice) {
			return y;
		}),
		vkh::selectPhysicalDevice(inst[0], deviceRating),
	};

    vk::Device logicalDevices[] = {
        vkh::createLogicalDevice(physicalDevices[0], {0, 2, 3}, {"extensions", "other ext"}),
        vkh::createLogicalDevice(physicalDevices[0], {1, 1, 3}, {"1ext"}),
        vkh::createLogicalDevice(physicalDevices[0], {0, 0, 0}, vectorCString),
    };
}

int main() {
}
