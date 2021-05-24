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
#include "renderer_system.h"
#include "renderer/quad_renderer.h"

using namespace ionengine;
using namespace ionengine::platform;

int32 main(int32, char**) {

    ecs::Engine ecs_engine;

    WindowEventLoop window_event_loop;
    Window window(L"Runtime", 800, 600, WindowStyle::Normal | WindowStyle::Minimize | WindowStyle::Maximaze, window_event_loop);

    renderer::RenderSystem::get_instance().create_renderer<renderer::QuadRenderer>(window);

    window_event_loop.run([&](const WindowEventHandler& event) -> void { 
        switch(event.event_type) {
            case WindowEvent::Closed: window_event_loop.exit(); break;
            case WindowEvent::KeyboardInput:
            case WindowEvent::MouseInput:
            case WindowEvent::MouseMoved: InputSystem::get_instance().on_event_handle(event); break;
            case WindowEvent::Updated: {
                
                auto input_sys = InputSystem::get_instance();
                if(input_sys.get_key_down(KeyCode::A)) {
                    std::cout << 1 << std::endl;
                }
                if(input_sys.get_key_up(KeyCode::A)) {
                    std::cout << 2 << std::endl;
                }

                InputSystem::get_instance().get_instance().tick();
                break;
            }
            default: break;
        }
    });

    std::wcout << "Exit from application" << std::endl;
    return 0;
}
