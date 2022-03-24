// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <engine/asset_manager.h>
#include <renderer/backend.h>
#include <renderer/renderer.h>
#include <lib/thread_pool.h>
#include <lib/exception.h>
#include <lib/hash/crc32.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    ThreadPool thread_pool(3);
    
    try {
        platform::WindowLoop loop;
        platform::Window window(u8"IONENGINE", 800, 600, false, loop);

        AssetManager asset_manager(thread_pool);
        
        renderer::Backend backend(0, renderer::SwapchainDesc { &window, 1, 2 }, "cache/shader_cache_d3d12.payload");

        Handle<AssetData> asset_data;
        asset_manager.load_asset_from_file("shader_package.asset"_hash, asset_data);
        auto& shader_package_data = std::get<renderer::ShaderPackageData>(asset_manager.get_asset_data(asset_data));

        renderer::Renderer renderer(backend, thread_pool, shader_package_data);
    
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

    } catch(Exception& e) {
        std::cerr << std::format("[Exception] {}", e.what()) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "[Debug] Main: Exit" << std::endl;

    thread_pool.join();
    return EXIT_SUCCESS;
}
