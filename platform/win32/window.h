// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class WindowsWindow : public Window {
public:

    WindowsWindow(const std::string& label, const uint32 width, const uint32 height, const WindowStyle window_style, WindowsWindowEventLoop& event_loop) : m_event_loop(event_loop) {
		
		WNDCLASS wnd_class{};
		wnd_class.lpszClassName = TEXT("WINAPI_WND");
		wnd_class.hInstance = GetModuleHandle(nullptr);
		wnd_class.lpfnWndProc = WindowsWindow::window_procedure;

		if (!RegisterClass(&wnd_class)) {
			throw std::runtime_error("An error occurred while registering the window");
		}

		m_wnd = CreateWindow(
			wnd_class.lpszClassName,
			stws(label).c_str(),
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

		m_id = GetWindowLong(m_wnd, GWL_ID);
		m_cursor = true;
		SetWindowLongPtr(m_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(m_wnd, SW_SHOWDEFAULT);

		m_devices[0].usUsagePage = 0x01;
		m_devices[0].usUsage = 0x06;
		m_devices[0].dwFlags = 0;
		m_devices[0].hwndTarget = m_wnd;

		m_devices[1].usUsagePage = 0x01;
		m_devices[1].usUsage = 0x02;
		m_devices[1].dwFlags = 0;
		m_devices[1].hwndTarget = m_wnd;

		if (!RegisterRawInputDevices(m_devices.data(), 2, sizeof(RAWINPUTDEVICE))) {
			throw std::runtime_error("An error occurred while registering raw input devices");
		}
    }

    ~WindowsWindow() {

		DestroyWindow(m_wnd);

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

    void* get_native_handle() const override { return reinterpret_cast<void*>(m_wnd); }

	uint64 get_id() const override { return m_id; }

	void show_cursor(const bool enable) override { m_cursor = enable; }
	void set_label(const std::string& label) override { SetWindowText(m_wnd, stws(label).c_str()); }

	void set_window_size(const uint32_t width, const uint32_t height) override { SetWindowPos(m_wnd, HWND_TOPMOST, 0, 0, width, height, 0); }
	
	PhysicalSize get_window_size() const override {
		RECT rect{};
		GetWindowRect(m_wnd, &rect);
		return { static_cast<uint32>(rect.right), static_cast<uint32>(rect.bottom) };
	}

	PhysicalSize get_client_size() const override { 
		RECT rect{};
		GetClientRect(m_wnd, &rect);
		return { static_cast<uint32>(rect.right), static_cast<uint32>(rect.bottom) }; 
	}

private:

	std::reference_wrapper<WindowsWindowEventLoop> m_event_loop;

    HWND m_wnd;
	std::array<RAWINPUTDEVICE, 2> m_devices;

    bool m_cursor;
	uint64_t m_id;

	static LRESULT window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		WindowEventHandler event_handler{};
		
		if (window) {
			event_handler.window_id = window->m_id;
		}

		switch(msg) {

			case WM_CLOSE: {
				event_handler.event_type = WindowEvent::Closed;
				window->m_event_loop.get().emplace_event(event_handler);
				break;
			}
			case WM_SIZE: {
				event_handler.event_type = WindowEvent::Sized;
				event_handler.event = PhysicalSize { LOWORD(lParam), HIWORD(lParam) };
				window->m_event_loop.get().emplace_event(event_handler);
				break;
			}
			case WM_MOUSEMOVE: {
				event_handler.event_type = WindowEvent::Moved;
				event_handler.event = MouseMoved { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), false };
				window->m_event_loop.get().emplace_event(event_handler);
				break;
			}
			case WM_INPUT: {
				byte buffer[48];
				uint32 size = 0;

				GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
				if (size <= 48) {
					GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));
				}

				RAWINPUT* raw_input = reinterpret_cast<RAWINPUT*>(buffer);
				DWORD device_type = raw_input->header.dwType;

				if (device_type == RIM_TYPEKEYBOARD) {
					event_handler.event_type = WindowEvent::KeyboardInput;

					USHORT key = raw_input->data.keyboard.VKey;
					USHORT flags = raw_input->data.keyboard.Flags;
					UINT msg = raw_input->data.keyboard.Message;

					switch (msg) {
						case WM_KEYDOWN:
						case WM_SYSKEYDOWN: event_handler.event = KeyboardInput { static_cast<uint32>(key), ElementState::Pressed }; break;
						case WM_KEYUP:
						case WM_SYSKEYUP: event_handler.event = KeyboardInput { static_cast<uint32>(key), ElementState::Released }; break;
					}
					
					window->m_event_loop.get().emplace_event(event_handler);
				} else if (device_type == RIM_TYPEMOUSE) {
					event_handler.event_type = WindowEvent::MouseInput;

					LONG x = raw_input->data.mouse.lLastX;
					LONG y = raw_input->data.mouse.lLastY;
					ULONG buttons = raw_input->data.mouse.ulButtons;
					USHORT flags = raw_input->data.mouse.usFlags;
					USHORT button_flags = raw_input->data.mouse.usButtonFlags;
					USHORT button_data = raw_input->data.mouse.usButtonData;

					switch (buttons) {
						case RI_MOUSE_LEFT_BUTTON_DOWN: event_handler.event = MouseInput { MouseButton::Left, ElementState::Pressed }; break;
						case RI_MOUSE_RIGHT_BUTTON_DOWN: event_handler.event = MouseInput { MouseButton::Right, ElementState::Pressed }; break;
						case RI_MOUSE_MIDDLE_BUTTON_DOWN: event_handler.event = MouseInput { MouseButton::Middle, ElementState::Pressed }; break;
						case RI_MOUSE_BUTTON_4_DOWN: event_handler.event = MouseInput { MouseButton::Four, ElementState::Pressed }; break;
						case RI_MOUSE_BUTTON_5_DOWN: event_handler.event = MouseInput { MouseButton::Five, ElementState::Pressed }; break;
						case RI_MOUSE_LEFT_BUTTON_UP: event_handler.event = MouseInput { MouseButton::Left, ElementState::Released }; break;
						case RI_MOUSE_RIGHT_BUTTON_UP: event_handler.event = MouseInput { MouseButton::Right, ElementState::Released }; break;
						case RI_MOUSE_MIDDLE_BUTTON_UP: event_handler.event = MouseInput { MouseButton::Middle, ElementState::Released }; break;
						case RI_MOUSE_BUTTON_4_UP: event_handler.event = MouseInput { MouseButton::Four, ElementState::Released }; break;
						case RI_MOUSE_BUTTON_5_UP: event_handler.event = MouseInput { MouseButton::Five, ElementState::Released }; break;
					}

					window->m_event_loop.get().emplace_event(event_handler);

					if (button_flags & RI_MOUSE_WHEEL) {
						event_handler.event_type = WindowEvent::MouseWheel;
						event_handler.event = MouseWheel { static_cast<float>(button_data / WHEEL_DELTA) };
						window->m_event_loop.get().emplace_event(event_handler);
					}

					event_handler.event_type = WindowEvent::MouseMoved;
					event_handler.event = MouseMoved { x, y, true };
					window->m_event_loop.get().emplace_event(event_handler);
				}
				break;
			}
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
    }
};

std::unique_ptr<Window> create_unique_window(const std::string& label, const uint32 width, const uint32 height, const WindowStyle window_style, WindowEventLoop& event_loop) {
	return std::make_unique<WindowsWindow>(label, width, height, window_style, static_cast<WindowsWindowEventLoop&>(event_loop));
}

int32 show_message_box(void* hwnd, const std::string& msg, const std::string& label, const MessageBoxStyle msgbox_style) {
    return MessageBox(reinterpret_cast<HWND>(hwnd), stws(msg).c_str(), stws(label).c_str(), static_cast<uint32>(msgbox_style));
}

}