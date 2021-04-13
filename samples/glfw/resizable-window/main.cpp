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
	auto glfwWindowPtr = glfwCreateWindow(helloTriangle.frameSizeX, helloTriangle.frameSizeY, "GLFW Resizable Hello Triangle", nullptr, nullptr);
	glfwSetWindowUserPointer(glfwWindowPtr, &helloTriangle);

	auto vkCApiSurfacePtr = static_cast<VkSurfaceKHR>(helloTriangle.vulkanWindowSurface);
	glfwCreateWindowSurface(helloTriangle.vulkanInstance, glfwWindowPtr, nullptr, &vkCApiSurfacePtr);
	helloTriangle.vulkanWindowSurface = vkCApiSurfacePtr;

	glfwSetWindowSizeCallback(glfwWindowPtr, [](GLFWwindow *glfwWindowPtr, int sizeX, int sizeY) -> void {
		auto &helloTriangle = *static_cast<HelloTriangle *>(glfwGetWindowUserPointer(glfwWindowPtr));

		helloTriangle.frameSizeX = sizeX;
		helloTriangle.frameSizeY = sizeY;

		helloTriangle.logicalDevice.waitIdle();

		helloTriangle.deinitSwapchain();

		if (!helloTriangle.vulkanWindowSurface) {
			auto vkCApiSurfacePtr = static_cast<VkSurfaceKHR>(helloTriangle.vulkanWindowSurface);
			glfwCreateWindowSurface(helloTriangle.vulkanInstance, glfwWindowPtr, nullptr, &vkCApiSurfacePtr);
			helloTriangle.vulkanWindowSurface = vkCApiSurfacePtr;

			if (!helloTriangle.selectedPhysicalDevice.getSurfaceSupportKHR(
					static_cast<std::uint32_t>(helloTriangle.queueIndices.presentation.value()),
					helloTriangle.vulkanWindowSurface))
				throw std::runtime_error("new surface does not support presentation on the previous queue index");
		}

		helloTriangle.initSwapchain();
		helloTriangle.initFramebuffers();

		helloTriangle.draw();
		glfwSwapBuffers(glfwWindowPtr);
	});

	helloTriangle.initDevice();
	helloTriangle.initVertexbuffer();
	helloTriangle.initSwapchain();
	helloTriangle.initFramebuffers();
	helloTriangle.initPipeline();

	while (true) {
		glfwPollEvents();
		if (glfwWindowShouldClose(glfwWindowPtr))
			break;

		helloTriangle.draw();
		glfwSwapBuffers(glfwWindowPtr);
	}

	helloTriangle.deinit();

} catch (const vk::SystemError &e) {
	fmt::print("vk::SystemError: {}", e.what());
} catch (const std::exception &e) {
	fmt::print("std::exception: {}", e.what());
} catch (...) {
	fmt::print("Unknown exception: no details available");
}
