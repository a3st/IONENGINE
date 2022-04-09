// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>
#include <renderer/context.h>
#include <renderer/mesh_renderer.h>
#include <lib/exception.h>

//#include <engine/sponza.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    //lib::ThreadPool thread_pool(3);
       
    try {
        platform::WindowLoop loop;
        platform::Window window("IONENGINE", 800, 600, false);

        renderer::Context context(window, 2);
        renderer::MeshRenderer mesh_renderer(context);

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;
                switch(event.type) {
                    case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; } break;
                    case platform::WindowEventType::Updated: {
                        context.render();
                    } break;
                }
            }
        );

    } catch(lib::Exception& e) {
        std::cerr << std::format("[Exception] {}", e.what()) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "[Debug] Main: Exit" << std::endl;

    //thread_pool.join();
    return EXIT_SUCCESS;
}
