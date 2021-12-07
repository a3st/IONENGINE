// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window_loop.h>

#define NOMINMAX
#define UNICODE
#include <windows.h>

using namespace ionengine::platform;

void WindowLoop::run(std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func) {

    MSG msg{};

    while(flow_ != WindowEventFlow::Exit) {

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        while(!events_.empty()) {
            WindowEvent event = events_.front();
            run_func(event, flow_);
            events_.pop();
        }

        WindowEvent post_event{ WindowEventType::Updated };
        run_func(post_event, flow_);

        if(flow_ == WindowEventFlow::Unknown || flow_ == WindowEventFlow::Exit) {
            break;
        }
    }
}

void WindowLoop::push_event(WindowEvent const& event) {

    events_.push(event);
}