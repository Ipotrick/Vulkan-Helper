#pragma once

struct WindowCreateInfo {
	int frameSizeX, frameSizeY;
	const char *title;
};

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan.hpp"

#include <bit>

class NativeWindow {
	bool closed = true;

protected:
	static inline HINSTANCE win32NativeApplicationInstanceHandle = nullptr;
	HWND handle = nullptr;

public:
	NativeWindow(const WindowCreateInfo &createInfo) {
		if (!win32NativeApplicationInstanceHandle) {
			// declare window class
			win32NativeApplicationInstanceHandle = GetModuleHandle(nullptr);
			WNDCLASSEX windowClass{
				.cbSize = sizeof(windowClass),
				.style = 0,
				.lpfnWndProc = [](HWND windowHandle, UINT messageId, WPARAM wp, LPARAM lp) -> LRESULT {
					NativeWindow *nativeWindowPtr = std::bit_cast<NativeWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
					if (nativeWindowPtr) {
						switch (messageId) {
						case WM_CLOSE: nativeWindowPtr->closed = true; break;
                        case WM_SIZE: nativeWindowPtr->onResize(LOWORD(lp), HIWORD(lp));
						}
					}
					return DefWindowProc(windowHandle, messageId, wp, lp);
				},
				.cbClsExtra = 0,
				.cbWndExtra = 0,
				.hInstance = win32NativeApplicationInstanceHandle,
				.hIcon = nullptr,
				.hCursor = nullptr,
				.hbrBackground = nullptr,
				.lpszMenuName = nullptr,
				.lpszClassName = "hello-triangle.NativeWindow",
				.hIconSm = nullptr,
			};
			RegisterClassEx(&windowClass);
		}
		RECT wr = {0, 0, createInfo.frameSizeX, createInfo.frameSizeY};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
		handle = CreateWindowEx(
			0, "hello-triangle.NativeWindow", createInfo.title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
			nullptr, nullptr, win32NativeApplicationInstanceHandle, nullptr);
		
		if (handle) {
			closed = false;
			SetWindowLongPtr(handle, GWLP_USERDATA, std::bit_cast<LONG_PTR>(this));
			ShowWindow(handle, SW_SHOW);
		}
	}
	auto createVulkanSurface(vk::Instance vkInstance) {
		return vkInstance.createWin32SurfaceKHR({
			.hinstance = win32NativeApplicationInstanceHandle,
			.hwnd = handle,
		});
	}
	bool isOpen() {
		return !closed;
	}
	void handleEvents() {
		MSG win32Message;
		while (PeekMessage(&win32Message, handle, 0, 0, PM_REMOVE)) {
			TranslateMessage(&win32Message);
			DispatchMessage(&win32Message);
		}
	}

    virtual void onResize(int, int) {}
};
#endif

using Window = NativeWindow;
