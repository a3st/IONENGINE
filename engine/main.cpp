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
        
        auto node_0 = test_scene.graph().add_node<scene::TransformNode>();
        node_0->position(lib::math::Vector3f(0.0f, 0.0f, 0.0f));

        test_scene.graph().root()->add_child(node_0);

        auto node_1 = test_scene.graph().add_node<scene::MeshNode>();
        node_1->position(lib::math::Vector3f(0.0f, 0.0, 0.0f));
        node_1->rotation(lib::math::Quaternionf::angle_axis(50.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        node_1->scale(lib::math::Vector3f(1.0f, 1.0f, 1.0f));

        node_0->add_child(node_1);

        auto camera = test_scene.graph().add_node<scene::CameraNode>();
        camera->position(lib::math::Vector3f(0.0f, 20.0f, 10.0f));

        /*std::cout << std::format("{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}", 
            node_1->transform_local().m00, node->transform_local().m01, node->transform_local().m02, node->transform_local().m03,
            node_1->transform_local().m10, node->transform_local().m11, node->transform_local().m12, node->transform_local().m13,
            node_1->transform_local().m20, node->transform_local().m21, node->transform_local().m22, node->transform_local().m23,
            node_1->transform_local().m30, node->transform_local().m31, node->transform_local().m32, node->transform_local().m33
        ) 
        << std::endl;

        test_scene.graph().update_hierarchical_data();

        std::cout << std::format("{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}", 
            node->transform_local().m00, node->transform_local().m01, node->transform_local().m02, node->transform_local().m03,
            node->transform_local().m10, node->transform_local().m11, node->transform_local().m12, node->transform_local().m13,
            node->transform_local().m20, node->transform_local().m21, node->transform_local().m22, node->transform_local().m23,
            node->transform_local().m30, node->transform_local().m31, node->transform_local().m32, node->transform_local().m33
        ) 
        << std::endl;*/

        AssetManager asset_manager;

        // asset::Material material("../../data/materials/default.json5", asset_manager);

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;
                switch(event.type) {
                    case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; } break;
                    case platform::WindowEventType::Updated: {
                        test_scene.graph().update_hierarchical_data();
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
