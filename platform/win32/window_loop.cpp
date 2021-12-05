// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include "window_loop.h"

using namespace ionengine::platform;

WindowLoop::WindowLoop() : quit_(false), event_{} {

}

void WindowLoop::run(const std::function<void(WindowEvent const&)>& run_func) {

    MSG msg{};
    WindowEvent post_event = { WindowEventType::Updated };

    while(!quit_) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            run_func(event_);
        }
        run_func(post_event);

        event_.type = WindowEventType::Unknown;
    }
}

void WindowLoop::set_event(WindowEvent const& event) {

    event_ = event;
}

void WindowLoop::quit() {

    quit_ = true;
}