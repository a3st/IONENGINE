// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <lib/exception.h>

#define NOMINMAX
#define UNICODE
#include <windows.h>

using namespace ionengine;
using namespace ionengine::platform;

struct Window::Impl {

	HWND hwnd;

	std::array<RAWINPUTDEVICE, 2> raw_devices;

	uint32_t _client_width;
	uint32_t _client_height;

	std::queue<WindowEvent> events;

	void initialize(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen);

	void deinitialize();

	uint32_t client_width() const;

	uint32_t client_height() const;

    void* native_handle() const;

    void label(std::string_view const label);

	std::queue<WindowEvent>& messages();

	static LRESULT wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

void Window::impl_deleter::operator()(Impl* ptr) const {

	delete ptr;
}

//===========================================================
//
//
//          Window Implementation
//
//
//===========================================================

void Window::Impl::initialize(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen) {

    WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE_WINDOW");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = Impl::wnd_proc;
	wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

    if (!RegisterClass(&wnd_class)) {
		throw lib::Exception("An error occurred while registering the window");
    }

	size_t length = strlen(reinterpret_cast<const char*>(label.data())) + 1;
    size_t result = 0;
	
	std::u16string out_str(length - 1, 0);

    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.data()), length - 1);

    hwnd = CreateWindow(
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

	if (!hwnd) {
		throw lib::Exception("An error occurred while creating the window");
	}

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	ShowWindow(hwnd, SW_SHOWDEFAULT);

	raw_devices[0].usUsagePage = 0x01;
	raw_devices[0].usUsage = 0x06;
	raw_devices[0].dwFlags = 0;
	raw_devices[0].hwndTarget = hwnd;

	raw_devices[1].usUsagePage = 0x01;
	raw_devices[1].usUsage = 0x02;
	raw_devices[1].dwFlags = 0;
	raw_devices[1].hwndTarget = hwnd;

	if (!RegisterRawInputDevices(raw_devices.data(), static_cast<uint32_t>(raw_devices.size()), sizeof(RAWINPUTDEVICE))) {
		throw std::runtime_error("An error occurred while registering raw input devices");
	}
}

void Window::Impl::deinitialize() {

	DestroyWindow(hwnd);

	raw_devices[0].usUsagePage = 0x01;
	raw_devices[0].usUsage = 0x06;
	raw_devices[0].dwFlags = RIDEV_REMOVE;
	raw_devices[0].hwndTarget = nullptr;

	raw_devices[1].usUsagePage = 0x01;
	raw_devices[1].usUsage = 0x02;
	raw_devices[1].dwFlags = RIDEV_REMOVE;
	raw_devices[1].hwndTarget = nullptr;

	RegisterRawInputDevices(raw_devices.data(), static_cast<uint32_t>(raw_devices.size()), sizeof(RAWINPUTDEVICE));
}

uint32_t Window::Impl::client_width() const {

	return _client_width;
}

uint32_t Window::Impl::client_height() const {

	return _client_height;
}

void* Window::Impl::native_handle() const {

	return reinterpret_cast<void*>(hwnd);
}

void Window::Impl::label(std::string_view const label) {

	size_t length = strlen(reinterpret_cast<const char*>(label.data())) + 1;
    size_t result = 0;

	std::u16string out_str(length - 1, 0);

    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.data()), length - 1);

	SetWindowText(hwnd, reinterpret_cast<const wchar_t*>(out_str.c_str()));
}

std::queue<WindowEvent>& Window::Impl::messages() {
	
	auto msg = MSG {};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	events.push(WindowEvent::updated());
	return events;
}

LRESULT Window::Impl::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	auto window_impl = reinterpret_cast<Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	if(!window_impl) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg) {

		case WM_CLOSE: {
			window_impl->events.push(WindowEvent::closed());
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

			window_impl->_client_width = std::max<uint32_t>(1, width - style_width);
			window_impl->_client_height = std::max<uint32_t>(1, height - style_height);

			window_impl->events.push(WindowEvent::sized(window_impl->_client_width, window_impl->_client_height));
		} break;

		case WM_INPUT: {

			uint8_t buffer[48];
			uint32_t size = 0;

			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			if (size <= 48) {
				GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));
			}

			RAWINPUT* raw_input = reinterpret_cast<RAWINPUT*>(buffer);
			DWORD device_type = raw_input->header.dwType;

			if(device_type == RIM_TYPEKEYBOARD) {
				
				auto window_event = WindowEvent {};

				USHORT key = raw_input->data.keyboard.VKey;
				USHORT flags = raw_input->data.keyboard.Flags;
				UINT msg = raw_input->data.keyboard.Message;

				switch (msg) {
					case WM_KEYDOWN:
					case WM_SYSKEYDOWN: {
						window_event = WindowEvent::keyboard_input(static_cast<uint32_t>(key), InputState::Pressed); 
					} break;
					case WM_KEYUP:
					case WM_SYSKEYUP: {
						window_event = WindowEvent::keyboard_input(static_cast<uint32_t>(key), InputState::Released); 
					} break;
				}
					
				window_impl->events.push(window_event);

			} else if (device_type == RIM_TYPEMOUSE) {

				LONG x = raw_input->data.mouse.lLastX;
				LONG y = raw_input->data.mouse.lLastY;

				window_impl->events.push(WindowEvent::mouse_moved(static_cast<int32_t>(x), static_cast<int32_t>(y)));
			}

		} break;
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//===========================================================
//
//
//          Window Pointer to Implementation
//
//
//===========================================================

Window::Window(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen) :
	_impl(std::unique_ptr<Impl, impl_deleter>(new Impl())) {

	_impl->initialize(label, width, height, fullscreen);
}

Window::~Window() {

	_impl->deinitialize();
}

Window::Window(Window&& other) noexcept {

	_impl = std::move(other._impl);
}

Window& Window::operator=(Window&& other) noexcept {

	_impl = std::move(other._impl);
	return *this;
}

uint32_t Window::client_width() const {
	return _impl->client_width();
}

uint32_t Window::client_height() const {
	return _impl->client_height();
}

void* Window::native_handle() const {
	return _impl->native_handle();
}

void Window::label(std::string_view const label) {
	_impl->label(label);
}

std::queue<WindowEvent>& Window::messages() {
	return _impl->messages();
}
