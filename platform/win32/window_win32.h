// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

namespace ionengine::platform {

enum class WindowStyle {
    Normal = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
    Minimize = WS_MINIMIZEBOX,
    Maximaze = WS_MAXIMIZEBOX
};

    //int32 operator|(const WindowStyle lhs, const WindowStyle rhs) {

    //}

class Window {
public:

    Window(const std::string_view& label, const uint32 width, const uint32 height, const WindowStyle window_style) {

    }

    ~Window() {

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

    bool m_cursor;
	uint64_t m_id;
	
    std::array<RAWINPUTDEVICE, 2> m_devices;

	static LRESULT window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    }

};

}