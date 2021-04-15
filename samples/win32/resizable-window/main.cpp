#include "../shared/window.hpp"
#include "../../shared/hello-triangle/app.hpp"

class ResizableHelloTriangleApp : public Window {
	HelloTriangle helloTriangle;

public:
	ResizableHelloTriangleApp() : Window({.frameSizeX = 800, .frameSizeY = 600, .title = "Win32 Resizable Hello Triangle"}) {
		helloTriangle.frameSizeX = 800;
		helloTriangle.frameSizeY = 600;
		helloTriangle.initInstance();
		helloTriangle.vulkanWindowSurface = Window::createVulkanSurface(*helloTriangle.vulkanInstance);
		helloTriangle.initRenderContext();
	}

	~ResizableHelloTriangleApp() {
		helloTriangle.deinit();
	}

	void draw() {
		helloTriangle.draw();
	}

	void onResize(int sizeX, int sizeY) {
		helloTriangle.frameSizeX = sizeX;
		helloTriangle.frameSizeY = sizeY;

		helloTriangle.deinitSwapchain();

		if (!helloTriangle.vulkanWindowSurface) {
			helloTriangle.vulkanWindowSurface = Window::createVulkanSurface(*helloTriangle.vulkanInstance);
			auto queueIndices = findQueueFamilyIndices(helloTriangle.selectedPhysicalDevice, helloTriangle.vulkanWindowSurface);
			if (!helloTriangle.selectedPhysicalDevice.getSurfaceSupportKHR(
					static_cast<std::uint32_t>(queueIndices.presentation.value()),
					helloTriangle.vulkanWindowSurface))
				throw std::runtime_error("new surface does not support presentation on the previous queue index");
		}

		helloTriangle.initSwapchain();
		helloTriangle.initFramebuffers();

		draw();
	}
};

int main() try {

	ResizableHelloTriangleApp app;

	using namespace std::chrono_literals;
	auto t0 = std::chrono::high_resolution_clock::now();
	std::size_t frameCount = 0;

	while (true) {
		++frameCount;
		auto now = std::chrono::high_resolution_clock::now();

		if (now - t0 > 1s) {
			t0 = now;
			fmt::print("frames/s: {}\n", frameCount);
			frameCount = 0;
		}

		app.handleEvents();
		if (!app.isOpen())
			break;

		app.draw();
	}

} catch (const vk::SystemError &e) {
	MessageBox(nullptr, e.what(), "vk::SystemError", MB_OK);
} catch (const std::exception &e) {
	MessageBox(nullptr, e.what(), "std::exception", MB_OK);
} catch (...) {
	MessageBox(nullptr, "no details available", "Unknown exception", MB_OK);
}
