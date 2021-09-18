// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/wnd/wnd.h"

// test lib
#include "lib/math.h"
#include "lib/memory.h"

#include "logger_system.h"
#include "input_system.h"
#include "render_system.h"

using namespace ionengine;

int32 main(int32, char**) {

    auto window_event_loop = platform::wnd::create_unique_window_event_loop();
    auto window = platform::wnd::create_unique_window("Runtime", 800, 600, platform::wnd::WindowStyle::Borderless, *window_event_loop);

    auto input_system = std::make_unique<InputSystem>();
    auto render_system = std::make_unique<RenderSystem>(*window);
    
    window_event_loop->run([&](const platform::wnd::WindowEventHandler& event) -> void { 
            
            switch(event.event_type) {
                case platform::wnd::WindowEvent::Closed: {
                    window_event_loop->exit(); 
                    break;
                }
                case platform::wnd::WindowEvent::Sized: {
                    auto event_size = std::get<platform::wnd::PhysicalSize>(event.event);
                    render_system->resize(event_size.width, event_size.height);
                    break;
                }
                case platform::wnd::WindowEvent::KeyboardInput:
                case platform::wnd::WindowEvent::MouseInput:
                case platform::wnd::WindowEvent::MouseMoved: {
                    input_system->on_event_handle(event); 
                    break;
                }
                case platform::wnd::WindowEvent::Updated: {
                        
                    if(input_system->get_key_down(KeyCode::A)) {
                        std::cout << 1 << std::endl;
                    }
                    if(input_system->get_key_up(KeyCode::A)) {
                            std::cout << 2 << std::endl;
                    }

                    render_system->tick();
                    input_system->tick();
                    break;
                }
            }
        }
    );
    return 0;
}