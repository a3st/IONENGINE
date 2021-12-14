// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>

#define NOMINMAX
#define UNICODE
#include <windows.h>

using namespace ionengine::platform;

struct Window::Impl {
	HWND hwnd;
	WindowLoop* loop;
	Size window_size;

	static LRESULT wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

LRESULT Window::Impl::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	auto window = reinterpret_cast<Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if(!window) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg) {
		case WM_CLOSE: {
			window->loop->push_event(WindowEvent{ WindowEventType::Closed });
			break;
		}
		/*case WM_SIZE: {
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
		}*/
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*void Window::set_label(std::string const& label) {

	size_t length = strlen(label.c_str()) + 1;
    assert(length > 0 && "length is less than 0 or equal 0");
    size_t result = 0;
	std::wstring out_str(length - 1, 0);
    mbstowcs_s(&result, out_str.data(), length, label.c_str(), length - 1);
	SetWindowText(reinterpret_cast<HWND>(hwnd_.get()), out_str.c_str());
}*/

Window::Window(std::u8string const& label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop) :
	impl_(std::make_unique<Impl>()) {

	impl_->loop = &loop;

    WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = Impl::wnd_proc;

    if (!RegisterClass(&wnd_class)) {
		throw std::runtime_error("An error occurred while registering the window");
    }

	size_t length = strlen(reinterpret_cast<const char*>(label.c_str())) + 1;
    assert(length > 0 && "length is less than 0 or equal 0");
    size_t result = 0;
	std::u16string out_str(length - 1, 0);
    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.c_str()), length - 1);

    impl_->hwnd = CreateWindow(
		wnd_class.lpszClassName,
		reinterpret_cast<const wchar_t*>(out_str.c_str()),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		0, 100,
		width, height,
		nullptr, 
		nullptr,
		wnd_class.hInstance,
		nullptr
	);

	if (!impl_->hwnd) {
		throw std::runtime_error("An error occurred while creating the window");
	}

    SetWindowLongPtr(impl_->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	ShowWindow(impl_->hwnd, SW_SHOWDEFAULT);
}

Window::~Window() = default;

Size Window::get_size() const {
	
	return { 800, 600 };
}

void* Window::get_handle() const {

	return impl_->hwnd;
}