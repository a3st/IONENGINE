// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window_loop.h>

using namespace ionengine::platform;

void WindowLoop::run(Window& window, std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func) {

    bool running = true;

    while(running) {

        auto& events = window.messages();

        while(!events.empty()) {
            WindowEvent event = events.front();
            run_func(event, _flow);
            events.pop();

            if(_flow == WindowEventFlow::Exit) {
                running = false;
                break;
            }
        }
    }
}
