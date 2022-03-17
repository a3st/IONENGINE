// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>

#include <engine/asset_manager.h>

#include <lib/thread_pool.h>
#include <lib/exception.h>
#include <lib/hash/crc32.h>

#include <renderer/backend.h>
#include <renderer/world_renderer.h>
#include <project/world_controller.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    ThreadPool thread_pool(3);
    
    try {
        platform::WindowLoop loop;
        platform::Window window(u8"IONENGINE", 800, 600, false, loop);

        renderer::Backend backend(0);

        AssetManager asset_manager(thread_pool);

        renderer::WorldRenderer world_renderer(backend, window, thread_pool);
        project::WorldController world_controller;

        if(!world_controller.initialize()) {
            throw Exception(u8"Error during initialize world controller");
        }

        Handle<AssetData> asset_data;
        asset_manager.load_asset_data<ShaderFile>("shader_package.asset"_hash, asset_data);

        //std::cout << data.file_path().string() << std::endl;
    
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
        std::cerr << std::format("[Exception] {}", e.what()) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Application (IONENGINE Runtime) is closed" << std::endl;

    thread_pool.join();
    return EXIT_SUCCESS;
}
