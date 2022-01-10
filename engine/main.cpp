// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>

#include <engine/thread_pool.h>
#include <engine/exception.h>

#include <renderer/backend.h>
#include <renderer/world_renderer.h>

using namespace ionengine;

int main(int*, char*) {

    platform::WindowLoop loop;
    platform::Window window(u8"IONENGINE", 800, 600, false, loop);

    ThreadPool thread_pool(3);

    renderer::Backend backend(0, &window, 2);
    renderer::WorldRenderer world_renderer(&backend, &thread_pool);
    
    try {
        loop.run(
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {

                flow = platform::WindowEventFlow::Poll;
                
                switch(event.type) {
                    case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; } break;
                    case platform::WindowEventType::Updated: { 
                        world_renderer.update();
                    } break;
                }
            }
        );
    } catch(Exception& e) {
        std::cerr << std::format("Exception: {}", e.what()) << std::endl;
    }

    thread_pool.join();
    return 0;
}