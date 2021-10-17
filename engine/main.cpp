// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/wnd/wnd.h"

using namespace ionengine;

int32 main(int32, char**) {

    auto window_event_loop = platform::wnd::create_unique_window_event_loop();

    auto window = platform::wnd::create_unique_window("IONENGINE", 800, 600, platform::wnd::WindowStyle::Borderless, window_event_loop.get());

    /*std::unique_ptr<EngineSystemPool> engine_system_pool = std::make_unique<EngineSystemPool>();

    struct Environment {
        inputsystem::InputSystem* inputsys;
        rendersystem::RenderSystem* rendersys;
    };

    Environment env{};*/

    
    /*try {
        env.inputsys = engine_system_pool->initialize_system<inputsystem::InputSystem>();
        env.rendersys = engine_system_pool->initialize_system<rendersystem::RenderSystem>(window.get());

    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }*/
    
    window_event_loop->run([&](const platform::wnd::WindowEventHandler& event) -> void {  
            try {
                switch(event.event_type) {
                    case platform::wnd::WindowEvent::Closed: {
                        window_event_loop->exit();
                        break;
                    }
                    case platform::wnd::WindowEvent::Sized: {
                        //auto event_size = std::get<platform::wnd::PhysicalSize>(event.event);
                        //env.rendersys->resize(event_size.width, event_size.height);
                        break;
                    }
                    case platform::wnd::WindowEvent::KeyboardInput:
                    case platform::wnd::WindowEvent::MouseInput:
                    case platform::wnd::WindowEvent::MouseMoved: {
                        //env.inputsys->on_event_handle(event); 
                        break;
                    }
                    case platform::wnd::WindowEvent::Updated: {
                    
                        /*if(env.inputsys->get_key_down(inputsystem::KeyCode::A)) {
                            std::cout << 1 << std::endl;
                        }
                        if(env.inputsys->get_key_up(inputsystem::KeyCode::A)) {
                            std::cout << 2 << std::endl;
                        }*/
                        //engine_system_pool->execute();
                        break;
                    }
                }
            } catch(std::exception& e) {
                std::cerr << e.what() << std::endl;
                window_event_loop->exit();
            }
        }
    );
    return 0;
}