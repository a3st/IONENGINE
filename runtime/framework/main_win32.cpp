// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/window.h"
#include "platform/msgbox.h"

// test lib
#include "lib/fmt.h"
#include "lib/math.h"
#include "lib/ecs.h"

#include "logger.h"
#include "input_system.h"
#include "renderer_system.h"

using namespace ionengine;
using namespace ionengine::platform;

int32 main(int32, char**) {

    try {

        ecs::Engine ecs_engine;

        WindowEventLoop window_event_loop;
        Window window(L"Runtime", 800, 600, WindowStyle::Normal | WindowStyle::Minimize | WindowStyle::Maximaze, window_event_loop);

        InputSystem input_system;
        RenderSystem render_system(window);
    
        window_event_loop.run([&](const WindowEventHandler& event) -> void { 
            switch(event.event_type) {
                case WindowEvent::Closed: {
                    window_event_loop.exit(); 
                    break;
                }
                case WindowEvent::Sized: {
                    auto event_size = std::get<PhysicalSize<uint32>>(event.event);
                    render_system.resize(event_size.width, event_size.height);
                    break;
                }
                case WindowEvent::KeyboardInput:
                case WindowEvent::MouseInput:
                case WindowEvent::MouseMoved: {
                    input_system.on_event_handle(event); 
                    break;
                }
                case WindowEvent::Updated: {
                    
                    if(input_system.get_key_down(KeyCode::A)) {
                        std::cout << 1 << std::endl;
                    }
                    if(input_system.get_key_up(KeyCode::A)) {
                        std::cout << 2 << std::endl;
                    }

                    //render_system.tick();
                    input_system.tick();
                    break;
                }
                default: break;
            }
        });
    }
    catch(std::exception& e) {
        show_message_box(nullptr, format<std::wstring>(L"Exception: {}", stws(e.what())), L"IONENGINE Crash", MessageBoxStyle::ButtonOK | MessageBoxStyle::IconError);
    }
    return 0;
}
