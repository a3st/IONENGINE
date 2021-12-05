// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "window.h"
#include "window_loop.h"

using namespace ionengine::platform;

void Window::set_label(std::string const& label) {

	size_t length = strlen(label.c_str()) + 1;
    assert(length > 0 && "length is less than 0 or equal 0");
    size_t result = 0;
	std::wstring out_str(length - 1, 0);
    mbstowcs_s(&result, out_str.data(), length, label.c_str(), length - 1);
	SetWindowText(reinterpret_cast<HWND>(hwnd_.get()), out_str.c_str());
}

Window::Window(std::string const& label, uint32_t const width, uint32_t const height, WindowLoop* const loop) :
	size_{ width, height }, loop_(loop)  {

    WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = Window::WndProc;

    if (!RegisterClass(&wnd_class)) {
		throw std::runtime_error("An error occurred while registering the window");
    }

	size_t length = strlen(label.c_str()) + 1;
    assert(length > 0 && "length is less than 0 or equal 0");
    size_t result = 0;
	std::wstring out_str(length - 1, 0);
    mbstowcs_s(&result, out_str.data(), length, label.c_str(), length - 1);

    hwnd_ = UniqueHWND(CreateWindow(
		wnd_class.lpszClassName,
		out_str.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		0, 100,
		width, height,
		nullptr, nullptr,
		wnd_class.hInstance,
		nullptr));

	if (!hwnd_) {
		throw std::runtime_error("An error occurred while creating the window");
	}

    SetWindowLongPtr(reinterpret_cast<HWND>(hwnd_.get()), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	ShowWindow(reinterpret_cast<HWND>(hwnd_.get()), SW_SHOWDEFAULT);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if(!window) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg) {
		case WM_CLOSE: {
			window->loop_->set_event(WindowEvent { WindowEventType::Closed });
			break;
		}
		case WM_SIZE: {
			if(wParam & SIZE_MINIMIZED) {
				break;
			}

			WORD width = LOWORD(lParam);
			WORD height = HIWORD(lParam);

			RECT style_rect{};
			AdjustWindowRect(&style_rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, false);
			LONG style_width = style_rect.right - style_rect.left;
			LONG style_height = style_rect.bottom - style_rect.top;

			window->size_ = Size { std::max<uint32_t>(1, width - style_width), std::max<uint32_t>(1, height - style_height) };

			window->loop_->set_event(WindowEvent { WindowEventType::Sized });
			break;
		}
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
