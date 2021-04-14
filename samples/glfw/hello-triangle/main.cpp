#include "../../shared/hello-triangle/app.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main() try {

	HelloTriangle helloTriangle;

	helloTriangle.frameSizeX = 800;
	helloTriangle.frameSizeY = 600;
	helloTriangle.initInstance();

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	auto glfwWindowPtr = glfwCreateWindow(helloTriangle.frameSizeX, helloTriangle.frameSizeY, "GLFW Hello Triangle", nullptr, nullptr);

	auto vkCApiSurfacePtr = static_cast<VkSurfaceKHR>(helloTriangle.vulkanWindowSurface);
	glfwCreateWindowSurface(helloTriangle.vulkanInstance, glfwWindowPtr, nullptr, &vkCApiSurfacePtr);
	helloTriangle.vulkanWindowSurface = vkCApiSurfacePtr;

	helloTriangle.initRenderContext();

	while (true) {
		glfwPollEvents();
		if (glfwWindowShouldClose(glfwWindowPtr))
			break;

		helloTriangle.draw();
	}

	helloTriangle.deinit();

} catch (const vk::SystemError &e) {
	fmt::print("vk::SystemError: {}", e.what());
} catch (const std::exception &e) {
	fmt::print("std::exception: {}", e.what());
} catch (...) {
	fmt::print("Unknown exception: no details available");
}
