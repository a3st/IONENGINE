// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <renderer/renderer.h>
#include <lib/thread_pool.h>
#include <lib/exception.h>
#include <lib/hash/crc32.h>

#include <scene/scene.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    lib::ThreadPool thread_pool(3);
    
    try {
        platform::WindowLoop loop;
        platform::Window window(u8"IONENGINE", 800, 600, false, loop);

#ifdef IONENGINE_RENDERER_BACKEND_D3D12
        std::filesystem::path const backend_cache_path = "cache/shader_cache_d3d12.bin";
#endif
    
        renderer::Renderer renderer(
            0, 
            renderer::backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = 2 },
            backend_cache_path,
            thread_pool
        );

        // Logic
        /*Handle<core::Asset> default_material_asset;
        asset_manager.load_asset_from_file(u8"content/default_material.asset", default_material_asset);

        Handle<core::Asset> default_shader_asset;
        asset_manager.load_asset_from_file(u8"content/default_shader.asset", default_shader_asset);

        scene::Scene default_scene;*/
        
        //scene::Material default_material(default_material_asset, default_shader_asset);

    
        loop.run(
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;
                switch(event.type) {
                    case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; } break;
                    case platform::WindowEventType::Updated: {
                        renderer.update();
                    } break;
                }
            }
        );

    } catch(lib::Exception& e) {
        std::cerr << std::format("[Exception] {}", e.what()) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "[Debug] Main: Exit" << std::endl;

    thread_pool.join();
    return EXIT_SUCCESS;
}
