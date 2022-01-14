// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <lib/exception.h>

#define NOMINMAX
#define UNICODE
#include <windows.h>

using namespace ionengine::platform;

struct Window::Impl {
	HWND hwnd;
	WindowLoop* loop;
	Size window_size;

	~Impl();
	static LRESULT wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

Window::Impl::~Impl() {

	DestroyWindow(hwnd);
}

LRESULT Window::Impl::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	auto window_impl = reinterpret_cast<Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	if(!window_impl) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg) {
		case WM_CLOSE: {
			window_impl->loop->push_event(
				WindowEvent { 
					WindowEventType::Closed
				}
			);
		} break;

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

			window_impl->window_size = Size { std::max<uint32_t>(1, width - style_width), std::max<uint32_t>(1, height - style_height) };

			window_impl->loop->push_event(
				WindowEvent {
					WindowEventType::Sized,
					window_impl->window_size
				}
			);
		} break;
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::set_label(std::u8string const& label) {

	size_t length = strlen(reinterpret_cast<const char*>(label.c_str())) + 1;
    size_t result{};
	std::u16string out_str(length - 1, 0);
    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.c_str()), length - 1);
	SetWindowText(_impl->hwnd, reinterpret_cast<const wchar_t*>(out_str.c_str()));
}

Window::Window(std::u8string const& label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop) :
	_impl(std::make_unique<Impl>()) {

	_impl->loop = &loop;

    WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE_WINDOW");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = Impl::wnd_proc;
	wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

    if (!RegisterClass(&wnd_class)) {
		throw Exception(u8"An error occurred while registering the window");
    }

	size_t length = strlen(reinterpret_cast<const char*>(label.c_str())) + 1;
    size_t result{};
	std::u16string out_str(length - 1, 0);
    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.c_str()), length - 1);

    _impl->hwnd = CreateWindow(
		wnd_class.lpszClassName,
		reinterpret_cast<const wchar_t*>(out_str.c_str()),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		0, 0,
		width, height,
		nullptr, 
		nullptr,
		wnd_class.hInstance,
		nullptr
	);

	if (!_impl->hwnd) {
		throw Exception(u8"An error occurred while creating the window");
	}

    SetWindowLongPtr(_impl->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(_impl.get()));
	ShowWindow(_impl->hwnd, SW_SHOWDEFAULT);
}

Window::~Window() = default;

Size Window::get_size() const {
	
	return _impl->window_size;
}

void* Window::get_handle() const {

	return _impl->hwnd;
}