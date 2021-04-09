#pragma once

struct WindowCreateInfo {
	int frameSizeX, frameSizeY;
	const char *title;
};

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

struct NativeWindow {
	HWND handle;
};

NativeWindow createNativeWindow(const WindowCreateInfo createInfo) {
    NativeWindow result;
    RECT wr = {0, 0, createInfo.frameSizeX, createInfo.frameSizeY};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    result.handle = CreateWindowEx(
        0, create_info.class_name, "my window title", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    return result;
}

#endif

class Window {
	NativeWindow nativeWindow;

public:
	Window(const WindowCreateInfo createInfo) {
	}
};
