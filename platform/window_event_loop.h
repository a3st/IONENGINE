// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class WindowEventLoop {
friend class Window;
public:

    WindowEventLoop() : m_running(false) {
    }

    void run(const std::function<void(const WindowEventHandler&)>& function_loop) {
        m_running = true;

        while (m_running) {
            MSG msg = {};
            while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            if (!m_events.empty()) {
                WindowEventHandler event_handler = std::move(m_events.back());
                m_events.pop();
                function_loop(event_handler);
            }

            // WindowEvent::Updated
            {
                WindowEventHandler event_handler = {};
                event_handler.window_id = ::GetWindowLong(msg.hwnd, GWL_ID);
                event_handler.event_type = WindowEvent::Updated;
                m_events.emplace(event_handler);
            }
	    }
    }
    
    void exit() {
        m_running = false;
    }

private:

    std::queue<WindowEventHandler> m_events;
	bool m_running;
};

}