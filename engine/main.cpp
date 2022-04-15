// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <lib/exception.h>

#include <shader/technique_parser.h>
#include <lib/algorithm.h>

//#include <engine/sponza.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    // lib::ThreadPool thread_pool(3);
       
    try {
        platform::WindowLoop loop;
        platform::Window window("IONENGINE", 800, 600, false);

        renderer::Renderer renderer(window);


        std::vector<uint8_t> bytes;
        size_t const file_size = lib::get_file_size("../../data/techniques/geometry.json5");
        bytes.resize(file_size);
        lib::load_bytes_from_file("../../data/techniques/geometry.json5", bytes);

        shader::TechniqueParser parser;
        parser.parse(bytes);

        scene::Scene test_scene;

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;
                switch(event.type) {
                    case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; } break;
                    case platform::WindowEventType::Updated: {
                        renderer.render(test_scene);
                    } break;
                    case platform::WindowEventType::Sized: {
                        auto event_size = std::get<platform::Size>(event.data);
                        renderer.resize(event_size.width, event_size.height);
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
