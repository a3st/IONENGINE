// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/wnd/wnd.h"

#include "engine_system_pool.h"

// test lib
#include "lib/math/math.h"
#include "lib/memory.h"
#include "lib/ini/ini.h"

#include "logger_system.h"
#include "inputsystem/inputsystem.h"
#include "render_system.h"

using namespace ionengine;

int32 main(int32, char**) {

    auto window_event_loop = platform::wnd::create_unique_window_event_loop();

    auto window = platform::wnd::create_unique_window("IONENGINE", 800, 600, platform::wnd::WindowStyle::Borderless, window_event_loop.get());

    std::unique_ptr<EngineSystemPool> engine_system_pool = std::make_unique<EngineSystemPool>();

    struct Environment {
        inputsystem::InputSystem* inputsys;
        RenderSystem* rendersys;
    };

    Environment env = {
        engine_system_pool->initialize_system<inputsystem::InputSystem>(),
        engine_system_pool->initialize_system<RenderSystem>(window.get())
    };
    
    window_event_loop->run([&](const platform::wnd::WindowEventHandler& event) -> void { 
            
            switch(event.event_type) {
                case platform::wnd::WindowEvent::Closed: {
                    window_event_loop->exit(); 
                    break;
                }
                case platform::wnd::WindowEvent::Sized: {
                    auto event_size = std::get<platform::wnd::PhysicalSize>(event.event);
                    env.rendersys->resize(event_size.width, event_size.height);
                    break;
                }
                case platform::wnd::WindowEvent::KeyboardInput:
                case platform::wnd::WindowEvent::MouseInput:
                case platform::wnd::WindowEvent::MouseMoved: {
                    env.inputsys->on_event_handle(event); 
                    break;
                }
                case platform::wnd::WindowEvent::Updated: {
                
                    if(env.inputsys->get_key_down(inputsystem::KeyCode::A)) {
                        std::cout << 1 << std::endl;
                    }
                    if(env.inputsys->get_key_up(inputsystem::KeyCode::A)) {
                        std::cout << 2 << std::endl;
                    }
                    engine_system_pool->execute();
                    break;
                }
            }
        }
    );
    return 0;
}