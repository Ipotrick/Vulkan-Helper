#include "../shared/window.hpp"
#include "../../shared/hello-triangle/app.hpp"

int main() try {

	HelloTriangle helloTriangle;

	helloTriangle.frameSizeX = 800;
	helloTriangle.frameSizeY = 600;
	helloTriangle.initInstance();
	Window window({.frameSizeX = helloTriangle.frameSizeX, .frameSizeY = helloTriangle.frameSizeY, .title = "Win32 Hello Triangle"});
	helloTriangle.vulkanWindowSurface = window.createVulkanSurface(helloTriangle.vulkanInstance);
	helloTriangle.initRenderContext();

	while (true) {
		window.handleEvents();
		if (!window.isOpen())
			break;

		helloTriangle.draw();
	}

	helloTriangle.deinit();

} catch (const vk::SystemError &e) {
	MessageBox(nullptr, e.what(), "vk::SystemError", MB_OK);
} catch (const std::exception &e) {
	MessageBox(nullptr, e.what(), "std::exception", MB_OK);
} catch (...) {
	MessageBox(nullptr, "no details available", "Unknown exception", MB_OK);
}
