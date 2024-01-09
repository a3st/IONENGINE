// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/window_loop.hpp"

using namespace ionengine::platform;

void WindowLoop::run(Window& window, std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func) {
    bool running = true;

    while(running) {
        auto& messages = window.get_queue_message();

        while(!messages.empty()) {
            WindowEvent event = messages.front();
            run_func(event, flow);
            messages.pop();

            if(flow == WindowEventFlow::Exit) {
                running = false;
                break;
            }
        }
    }
}