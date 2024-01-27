// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "engine.hpp"
#include "renderer/pipelines/my_render_pipeline.hpp"
#include "asset/model.hpp"

using namespace ionengine;

Engine::Engine(
    std::string_view const title
) {
    window_loop = core::make_ref<platform::WindowLoop>();
    window = platform::Window::create(title, 800, 600, false);

    auto render_pipeline = core::make_ref<renderer::MyRenderPipeline>();
    renderer = core::make_ref<renderer::Renderer>(render_pipeline, &window);

    /*std::string shader_bytes;
    {
        std::ifstream ifs("shaders/3d.wgsl");
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        shader_bytes.resize(size);
        ifs.read(reinterpret_cast<char* const>(shader_bytes.data()), size);
    }
    
    std::vector<renderer::ShaderData> shaders;
    shaders.emplace_back(
        renderer::ShaderData {
            .shader_name = "3d",
            .shader_code = shader_bytes
        }
    );
    renderer->load_shaders(shaders);*/
}

auto Engine::run() -> void {

    //auto main_camera = renderer->create_camera(renderer::CameraProjectionType::Perspective);

    //std::vector<core::ref_ptr<renderer::Camera>> targets;
    //targets.emplace_back(main_camera);

    std::vector<uint8_t> object_bytes;
    {
        std::ifstream ifs("models/vehicle-1mat.glb", std::ios::binary);
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        object_bytes.resize(size);
        ifs.read(reinterpret_cast<char* const>(object_bytes.data()), size);
    }
    
    Model model(object_bytes, ModelFormat::GLB);

    window_loop->run(
        &window, 
        [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
            flow = platform::WindowEventFlow::Poll;

            auto event_visitor = make_visitor(
                [&](platform::WindowEventData<platform::WindowEventType::Closed> const& data) {
                    flow = platform::WindowEventFlow::Exit;
                },
                [&](platform::WindowEventData<platform::WindowEventType::Updated> const& data) {

                    renderer->update(0.1f);
                    
                    for(uint32_t j = 5; j < 6; ++j) {
                        for(uint32_t i = 0; i < model.get_mesh(j).primitives.size(); ++i) {
                            auto primitive_data = renderer::PrimitiveData {
                                .vertices = model.get_mesh(j).primitives[i].vertices,
                                .indices = model.get_mesh(j).primitives[i].indices,
                                .hash = model.get_mesh(j).primitives[i].hash
                            };

                            auto render_task_data = renderer::RenderTaskData {
                                .primitive = primitive_data,
                                .index_count = model.get_mesh(j).index_counts[i]
                            };
                            renderer->add_render_task(render_task_data);
                        }
                    }

                    renderer->render();
                },
                [&](platform::WindowEventData<platform::WindowEventType::Sized> const& data) {      
                    renderer->resize(data.width, data.height);
                },
                [&](platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& data) {

                },
                [&](platform::WindowEventData<platform::WindowEventType::MouseMoved> const& data) {

                },
                [&](platform::WindowEventData<platform::WindowEventType::MouseInput> const& data) {

                }
            );

            std::visit(event_visitor, event.data);
        }
    );
}