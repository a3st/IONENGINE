// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/api.h"

// test lib
#include "lib/fmt.h"
#include "lib/math.h"
#include "lib/memory.h"

#include "logger.h"
#include "input_system.h"
#include "render_system.h"

using namespace ionengine;
using namespace ionengine::platform;

int32 main(int32, char**) {

    auto window_event_loop = create_unique_window_event_loop();
    auto window = create_unique_window("Runtime", 800, 600, WindowStyle::Normal | WindowStyle::Minimize | WindowStyle::Maximaze, *window_event_loop);

    auto input_system = std::make_unique<InputSystem>();
    auto render_system = std::make_unique<RenderSystem>(*window);
    
    try {
        window_event_loop->run([&](const WindowEventHandler& event) -> void { 
            switch(event.event_type) {
                case WindowEvent::Closed: {
                    window_event_loop->exit(); 
                    break;
                }
                case WindowEvent::Sized: {
                    auto event_size = std::get<PhysicalSize>(event.event);
                    render_system->resize(event_size.width, event_size.height);
                    break;
                }
                case WindowEvent::KeyboardInput:
                case WindowEvent::MouseInput:
                case WindowEvent::MouseMoved: {
                    input_system->on_event_handle(event); 
                    break;
                }
                case WindowEvent::Updated: {
                    
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
        });
    }
    catch(std::exception& e) {
        show_message_box(nullptr, format<char>("Exception: {}", e.what()), "IONENGINE Crash", MessageBoxStyle::ButtonOK | MessageBoxStyle::IconError);
    }
    return 0;
}