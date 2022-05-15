// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <scene/mesh_node.h>
#include <scene/camera_node.h>
#include <lib/exception.h>
#include <lib/algorithm.h>
#include <lib/thread_pool.h>

#include <asset/asset_manager.h>
#include <asset/material.h>

using namespace ionengine;

int main(int* argc, char** agrv) {

    lib::ThreadPool thread_pool(3);

    try {
        platform::WindowLoop loop;
        platform::Window window("IONENGINE", 800, 600, false);
        asset::AssetManager asset_manager(thread_pool);
        renderer::Renderer renderer(window, asset_manager);

        {
            asset::AssetPtr<asset::Mesh> mesh = asset_manager.get_mesh("unpacked/objects/cube.obj");
            asset::AssetPtr<asset::Material> material = asset_manager.get_material("../../data/materials/default.json5");

            mesh.wait();
            material.wait();
        }

        float rotate = 0.0f;
        scene::Scene test_scene;
        {
            auto node_0 = test_scene.graph().add_node<scene::TransformNode>();
            node_0->position(lib::math::Vector3f(0.0f, 1.0f, 0.0f));

            test_scene.graph().root()->add_child(node_0);

            auto node_1 = test_scene.graph().add_node<scene::MeshNode>();
            node_1->name("mesh");
            node_1->mesh(asset_manager.get_mesh("unpacked/objects/cube.obj"));
            node_1->material(0, asset_manager.get_material("../../data/materials/default.json5"));
            node_1->position(lib::math::Vector3f(0.0f, -0.5f, 0.0f));
            node_1->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
            node_1->scale(lib::math::Vector3f(1.0f, 1.0f, 1.0f));

            node_0->add_child(node_1);

            auto camera = test_scene.graph().add_node<scene::CameraNode>();
            camera->name("MainCamera");
            camera->position(lib::math::Vector3f(0.0f, 2.0f, 3.0f));
        }

        auto begin_time = std::chrono::high_resolution_clock::now();
        uint64_t frame_count = 0;

        float frame_timer = 0.0f;

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;
                switch(event.type) {
                    case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; } break;
                    case platform::WindowEventType::Updated: {
                        auto end_time = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<float> delta_time = end_time - begin_time;
                        begin_time = end_time;

                        /*
                        rotate += delta_time.count();
                        if(rotate >= 180.0f) {
                            rotate = 0.0f;
                        }
                        auto mesh = test_scene.graph().find_by_name<scene::MeshNode>("mesh");
                        mesh->rotation(lib::math::Quaternionf::angle_axis(rotate, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
                        */

                        frame_timer += delta_time.count();
                        if(frame_timer >= 1.0f) {
                            window.label(std::format("{} FPS, {} DT", frame_count, delta_time.count()));
                            frame_timer = 0.0f;
                            frame_count = 0;
                        }

                        test_scene.graph().update_hierarchical_data();

                        asset_manager.update(delta_time.count());
                        renderer.update(delta_time.count());
                        renderer.render(test_scene);

                        ++frame_count;
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

    thread_pool.join();
    return EXIT_SUCCESS;
}
