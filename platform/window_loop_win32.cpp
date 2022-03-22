// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window_loop.h>

#define NOMINMAX
#define UNICODE
#include <windows.h>

using namespace ionengine::platform;

void WindowLoop::run(std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func) {

    auto msg = MSG {};

    while(true) {

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        while(!_events.empty()) {
            WindowEvent event = _events.front();
            run_func(event, _flow);
            _events.pop();
        }

        if(_flow == WindowEventFlow::Exit) {
            break;
        }

        auto post_event = WindowEvent { WindowEventType::Updated };
        run_func(post_event, _flow);
    }
}
