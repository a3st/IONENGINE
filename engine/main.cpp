// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>

#include <lib/thread_pool.h>
#include <lib/exception.h>

#include <renderer/backend.h>
#include <renderer/world_renderer.h>
#include <project/world_controller.h>

using namespace ionengine;

int main(int*, char*) {

    ThreadPool thread_pool(3);
    
    try {
        platform::WindowLoop loop;
        platform::Window window(u8"IONENGINE", 800, 600, false, loop);

        renderer::Backend backend(0, &window, 2);
        renderer::WorldRenderer world_renderer(&backend, &thread_pool);
        project::WorldController world_controller;

        if(!world_controller.initialize()) {
            throw Exception(u8"Error during initialize world controller");
        }
    
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

        backend.wait_for_idle_device();

    } catch(Exception& e) {
        std::cerr << std::format("[Exception] {}", e.what()) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Exit app" << std::endl;

    thread_pool.join();
    return EXIT_SUCCESS;
}