// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <scene/mesh_node.h>
#include <lib/exception.h>
#include <lib/algorithm.h>

#include <engine/asset_manager.h>
#include <asset/material.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    // lib::ThreadPool thread_pool(3);

    try {
        platform::WindowLoop loop;
        platform::Window window("IONENGINE", 800, 600, false);

        renderer::Renderer renderer(window);

        scene::Scene test_scene;
        test_scene.push_node<scene::MeshNode>();

        AssetManager asset_manager;

        // asset::Material material("../../data/materials/default.json5", asset_manager);

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
