// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

namespace ionengine::platform {

enum class WindowStyle : uint32 {
    Normal = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
    Minimize = WS_MINIMIZEBOX,
    Maximaze = WS_MAXIMIZEBOX
};

WindowStyle operator|(const WindowStyle lhs, const WindowStyle rhs) {
	return static_cast<WindowStyle>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

class Window {
public:

    Window(const std::wstring_view& label, const uint32 width, const uint32 height, const WindowStyle window_style, WindowEventLoop& event_loop) : 
		m_event_loop(event_loop) {
		
		WNDCLASS wnd_class = {};
		wnd_class.lpszClassName = TEXT("WINAPI_WND");
		wnd_class.hInstance = ::GetModuleHandle(nullptr);
		wnd_class.lpfnWndProc = Window::window_procedure;

		if (!::RegisterClass(&wnd_class)) {
			throw std::runtime_error("An error occurred while registering the window");
		}

		m_wnd = ::CreateWindow(
			wnd_class.lpszClassName,
			label.data(),
			static_cast<uint32>(window_style),
			0, 100,
			width, height,
			nullptr, nullptr,
			wnd_class.hInstance,
			nullptr
		);

		if (!m_wnd) {
			throw std::runtime_error("An error occurred while creating the window");
		}

		m_id = ::GetWindowLong(m_wnd, GWL_ID);
		m_cursor = true;
		::SetWindowLongPtr(m_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		::ShowWindow(m_wnd, SW_SHOWDEFAULT);

		m_devices[0].usUsagePage = 0x01;
		m_devices[0].usUsage = 0x06;
		m_devices[0].dwFlags = 0;
		m_devices[0].hwndTarget = m_wnd;

		m_devices[1].usUsagePage = 0x01;
		m_devices[1].usUsage = 0x02;
		m_devices[1].dwFlags = 0;
		m_devices[1].hwndTarget = m_wnd;

		if (!::RegisterRawInputDevices(m_devices.data(), 2, sizeof(RAWINPUTDEVICE))) {
			throw std::runtime_error("An error occurred while registering raw input devices");
		}
    }

    ~Window() {
		::DestroyWindow(m_wnd);

		m_devices[0].usUsagePage = 0x01;
		m_devices[0].usUsage = 0x06;
		m_devices[0].dwFlags = RIDEV_REMOVE;
		m_devices[0].hwndTarget = nullptr;

		m_devices[1].usUsagePage = 0x01;
		m_devices[1].usUsage = 0x02;
		m_devices[1].dwFlags = RIDEV_REMOVE;
		m_devices[1].hwndTarget = nullptr;

		::RegisterRawInputDevices(m_devices.data(), 2, sizeof(RAWINPUTDEVICE));
    }

    void* get_handle() const {
		return reinterpret_cast<void*>(m_wnd);
	}

	void show_cursor(const bool enable) {
		m_cursor = enable;
	}

	void set_label(const std::wstring& label) {
		//::SetWindowText(m_wnd, label.c_str());
	}

	void set_window_size(const uint32_t width, const uint32_t height) {
		::SetWindowPos(m_wnd, HWND_TOPMOST, 0, 0, width, height, 0);
	}

	uint64 get_id() const {
		return m_id;
	}

private:

    HWND m_wnd;
	WindowEventLoop& m_event_loop;

    bool m_cursor;
	uint64_t m_id;
	
    std::array<RAWINPUTDEVICE, 2> m_devices;

	static LRESULT window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		Window* window = reinterpret_cast<Window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		WindowEventHandler event_handler = {};
		
		if (window) {
			event_handler.window_id = window->m_id;
		}

		switch(msg) {
			case WM_CLOSE: {
				event_handler.event_type = WindowEvent::Closed;
				window->m_event_loop.m_events.emplace(event_handler);
				break;
			}
			case WM_SIZE: {
				event_handler.event_type = WindowEvent::Sized;
				event_handler.event = PhysicalSize<uint32_t>(LOWORD(lParam), HIWORD(lParam));
				window->m_event_loop.m_events.emplace(event_handler);
				break;
			}
			case WM_MOUSEMOVE: {
				event_handler.event_type = WindowEvent::Moved;
				event_handler.event = MouseMoved(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), false);
				window->m_event_loop.m_events.emplace(event_handler);
				break;
			}
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

};

}