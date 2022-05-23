// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>
#include <renderer/renderer.h>
#include <asset/asset_manager.h>
#include <input/input_manager.h>
#include <scene/scene.h>
#include <scene/mesh_node.h>
#include <scene/camera_node.h>
#include <lib/exception.h>
#include <lib/algorithm.h>
#include <lib/thread_pool.h>

#include <engine/framework.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    lib::ThreadPool thread_pool(3);

    try {
        platform::WindowLoop loop;
        platform::Window window("IONENGINE", 800, 600, false);
        
        asset::AssetManager asset_manager(thread_pool);

        auto render_asset_desc = renderer::DefaultAssetDesc {
            .deffered = asset_manager.get_technique("engine/techniques/deffered.json5"),
            .lighting = asset_manager.get_technique("engine/techniques/lighting.json5"),
            .billboard = asset_manager.get_technique("engine/techniques/billboard.json5"),
            .quad = asset_manager.get_mesh("engine/quad.obj")
        };
        renderer::Renderer renderer(window, asset_manager, render_asset_desc);

        renderer.editor_mode(false);
        
        input::InputManager input_manager;

        window.cursor(false);

        window.label("IONENGINE content/levels/city17.json5");
        framework::Framework framework(asset_manager, input_manager);

        auto begin_time = std::chrono::high_resolution_clock::now();
        uint64_t frame_count = 0;

        float frame_timer = 0.0f;

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;

                auto event_visitor = make_visitor(
                    [&](platform::WindowEventData<platform::WindowEventType::Closed> const& data) {
                        flow = platform::WindowEventFlow::Exit;
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::Updated> const& data) {
                        auto end_time = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<float> delta_time = end_time - begin_time;
                        begin_time = end_time;

                        framework.update(delta_time.count());

                        frame_timer += delta_time.count();
                        if(frame_timer >= 1.0f) {
                            window.label(std::format("IONENGINE KidsRoom Demo [{} fps]", frame_count, delta_time.count()));
                            frame_timer = 0.0f;
                            frame_count = 0;
                        }

                        if(input_manager.key_down(input::KeyCode::Escape)) {
                            flow = platform::WindowEventFlow::Exit;
                        }

                        if(input_manager.key_down(input::KeyCode::F1)) {
                            window.cursor(false);
                        }

                        if(input_manager.key_down(input::KeyCode::F2)) {
                            window.cursor(true);
                        }

                        if(input_manager.key_down(input::KeyCode::F3)) {
                            renderer.editor_mode(true);
                        }

                        if(input_manager.key_down(input::KeyCode::F4)) {
                            renderer.editor_mode(false);
                        }

                        input_manager.update();
                        framework.scene().graph().update_hierarchical_data();
                        asset_manager.update(delta_time.count());
                        renderer.update(delta_time.count());
                        renderer.render(framework.scene());

                        ++frame_count;
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::Sized> const& data) {
                        renderer.resize(data.width, data.height);

                        scene::CameraNode* camera = framework.scene().graph().find_by_name<scene::CameraNode>("MainCamera");
                        camera->aspect_ratio(data.width / static_cast<float>(data.height));
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& data) {
                        input_manager.on_keyboard_event(data);
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::MouseMoved> const& data) {
                        input_manager.on_mouse_moved_event(data);
                    }
                );

                std::visit(event_visitor, event.data);
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
