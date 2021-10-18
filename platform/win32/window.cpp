// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "window.h"

using namespace ionengine::platform;

Window::Window() {

}

Window::~Window() {

	DestroyWindow(hwnd_);
}

Window::Window(const std::string& label, const uint32_t width, const uint32_t height) {

    WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = Window::WndProc;

    if (!RegisterClass(&wnd_class)) {
		throw std::runtime_error("An error occurred while registering the window");
    }

    hwnd_ = CreateWindow(
		wnd_class.lpszClassName,
		L"123",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		0, 100,
		width, height,
		nullptr, nullptr,
		wnd_class.hInstance,
		nullptr);

	if (!hwnd_) {
		throw std::runtime_error("An error occurred while creating the window");
	}

    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	ShowWindow(hwnd_, SW_SHOWDEFAULT);
}

void* Window::GetNativeHandle() const {

    return reinterpret_cast<HWND>(hwnd_);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
