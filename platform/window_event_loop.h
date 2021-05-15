// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class WindowEventLoop {
friend class Window;
public:

    WindowEventLoop() : m_running(false) {
    }

    void run(const std::function<void(const WindowEvent&)>& function_loop) {
        while (!m_running) {
            MSG msg = {};
            while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            
            if (!m_events.empty()) {
                WindowEvent event = std::move(m_events.back());
                m_events.pop();
                function_loop(event);
            }
	    }
    }
    
    void exit() {
        m_running = false;
    }

private:

    std::queue<WindowEvent> m_events;
	bool m_running;
};

}