// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class WindowStyle : uint32 {
    Normal = 0
};

WindowStyle operator|(const WindowStyle lhs, const WindowStyle rhs) {
	return static_cast<WindowStyle>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

class Window final {
public:

    Window(const std::wstring_view& label, const uint32 width, const uint32 height, const WindowStyle window_style, WindowEventLoop& event_loop) : 
		m_event_loop(event_loop) {


    }

private:

    WindowEventLoop& m_event_loop;

    bool m_cursor;
	uint64_t m_id;
};

}