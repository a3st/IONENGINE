// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform::wnd {

class WindowsWindowEventLoop : public WindowEventLoop {
public:

    WindowsWindowEventLoop() : m_running(false) {

    }

    void run(const std::function<void(const WindowEventHandler&)>& function_loop) override {

        m_running = true;
        while (m_running) {

            MSG msg{};
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            while(!m_events.empty()) {
                WindowEventHandler event_handler = std::move(m_events.back());
                m_events.pop();
                //std::cout << "events count: " << m_events.size() << std::endl;
                function_loop(event_handler);
            }

            // WindowEvent::Updated
            {
                WindowEventHandler event_handler{};
                event_handler.window_id = GetWindowLong(msg.hwnd, GWL_ID);
                event_handler.event_type = WindowEvent::Updated;
                function_loop(event_handler);
            }
	    }
    }
    
    void exit() override { m_running = false; }

    void emplace_event(const WindowEventHandler& event) {
        m_events.emplace(event);
    }

private:

    std::queue<WindowEventHandler> m_events;
	bool m_running;
};

std::unique_ptr<WindowEventLoop> create_unique_window_event_loop() {
	return std::make_unique<WindowsWindowEventLoop>();
}

}