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
    window = core::make_ref<platform::Window>(title, 800, 600, false);

    auto render_pipeline = core::make_ref<renderer::MyRenderPipeline>();
    renderer = core::make_ref<renderer::Renderer>(render_pipeline, &window);
}

auto Engine::run() -> void {

    auto main_camera = renderer->create_camera();

    std::vector<core::ref_ptr<renderer::Camera>> targets;
    targets.emplace_back(main_camera);

    std::vector<uint8_t> cube_bytes;
    {
        std::ifstream ifs("models/vehicle.glb", std::ios::binary);
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        cube_bytes.resize(size);
        ifs.read(reinterpret_cast<char* const>(cube_bytes.data()), size);
    }
    
    Model model(cube_bytes, ModelFormat::GLB);

    window_loop->run(
        &window, 
        [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
            flow = platform::WindowEventFlow::Poll;

            auto event_visitor = make_visitor(
                [&](platform::WindowEventData<platform::WindowEventType::Closed> const& data) {
                    flow = platform::WindowEventFlow::Exit;
                },
                [&](platform::WindowEventData<platform::WindowEventType::Updated> const& data) {
                    
                    for(auto const& primitive : model.get_mesh(0).primitives) {
                        auto primitive_data = renderer::PrimitiveData {
                            .vertices = primitive.vertices,
                            .indices = primitive.indices
                        };

                        renderer->add_render_task(primitive_data);
                    }

                    renderer->render(targets);
                },
                [&](platform::WindowEventData<platform::WindowEventType::Sized> const& data) {      
                    renderer->resize(&window, data.width, data.height);
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