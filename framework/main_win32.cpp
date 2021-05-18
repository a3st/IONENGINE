// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/window.h"

// test lib
#include "lib/fmt.h"
#include "lib/math.h"
#include "lib/ecs.h"
#include "lib/sparse_set.h"

#include "logger.h"
#include "input_system.h"

using namespace ionengine;
using namespace ionengine::platform;

int32 main(int32, char**) {

    ecs::Engine ecs_engine;

    WindowEventLoop window_event_loop;
    Window window(L"Window", 800, 600, WindowStyle::Normal | WindowStyle::Minimize | WindowStyle::Maximaze, window_event_loop);

    window_event_loop.run([&](const WindowEventHandler& event) -> void { 
        switch(event.event_type) {
            case WindowEvent::Closed: window_event_loop.exit(); break;
            case WindowEvent::KeyboardInput:
            case WindowEvent::MouseInput: InputSystem::get_instance().on_key_update(event); break;
            case WindowEvent::Updated: {
               
                break;
            }
            default: break;
        }
    });

    std::wcout << "Exit from application" << std::endl;
    return 0;
}
