// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "asset/ktx2_image.hpp"
#include "asset/model.hpp"
#include "precompiled.h"
#include "renderer/pipelines/my_render_pipeline.hpp"

namespace ionengine
{
    Engine::Engine(std::string_view const title)
        : window_loop(core::make_ref<platform::WindowLoop>()), window(platform::Window::create(title, 800, 600, false)),
          renderer(core::make_ref<renderer::Renderer>(core::make_ref<renderer::MyRenderPipeline>(), &window))
    {
    }

    auto Engine::run() -> void
    {
        auto main_camera = renderer->create_camera(renderer::CameraProjectionType::Perspective, window->get_width(),
                                                   window->get_height());
        std::vector<core::ref_ptr<renderer::Camera>> targets;
        targets.emplace_back(main_camera);

        std::vector<uint8_t> object_bytes;
        {
            std::ifstream ifs("models/vehicle-1mat.glb", std::ios::binary);
            ifs.seekg(0, std::ios::end);
            auto size = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            object_bytes.resize(size);
            ifs.read(reinterpret_cast<char* const>(object_bytes.data()), size);
        }

        Model model(&renderer, object_bytes, ModelFormat::GLB);

        std::vector<uint8_t> image_bytes;
        {
            std::ifstream ifs("textures/bird.ktx2", std::ios::binary);
            ifs.seekg(0, std::ios::end);
            auto size = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            image_bytes.resize(size);
            ifs.read(reinterpret_cast<char* const>(image_bytes.data()), size);
        }

        ktx2::Ktx2Image ktx2(image_bytes);

        auto image_data = ktx2.get_image_data();

        auto texture = renderer->create_texture(image_data.pixel_width, image_data.pixel_height, 1, 3,
                                                renderer::rhi::TextureFormat::BC3, renderer::rhi::TextureDimension::_2D,
                                                {ktx2.get_mip_data(0), ktx2.get_mip_data(1), ktx2.get_mip_data(2)});

        auto rot = math::Quaternionf::angle_axis(0.0f, math::Vector3f(0.0f, 1.0f, 0.0f)) *
                   math::Quaternionf::angle_axis(0.0f, math::Vector3f(0.0f, 1.0f, 0.0f)) *
                   math::Quaternionf::angle_axis(0.0f, math::Vector3f(1.0f, 0.0f, 0.0f));

        window_loop->run(&window, [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
            flow = platform::WindowEventFlow::Poll;

            auto event_visitor = make_visitor(
                [&](platform::WindowEventData<platform::WindowEventType::Closed> const& data) {
                    flow = platform::WindowEventFlow::Exit;
                },
                [&](platform::WindowEventData<platform::WindowEventType::Updated> const& data) {
                    main_camera->calculate(math::Vector3f(5.0f, 5.0f, 5.0f), rot);

                    for (uint32_t j = 0; j < model.get_size(); ++j)
                    {
                        for (uint32_t i = 0; i < model.get_mesh(j).primitives.size(); ++i)
                        {
                            auto render_task_data = renderer::RenderTaskData{
                                .primitive = model.get_mesh(j).primitives[i],
                                .model = math::Matrixf::scale(math::Vector3f(3.0f, 3.0f, 3.0f)) *
                                         math::Matrixf::translate(math::Vector3f(0.0f, 0.0f, 0.0f)),
                                .shader = 0,
                                .texture = texture,
                                .mask = (uint8_t)renderer::MyRenderMask::Opaque};
                            renderer->tasks() << render_task_data;
                        }
                    }

                    renderer->render(targets);
                },
                [&](platform::WindowEventData<platform::WindowEventType::Sized> const& data) {
                    renderer->resize(data.width, data.height);
                },
                [&](platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& data) {

                },
                [&](platform::WindowEventData<platform::WindowEventType::MouseMoved> const& data) {

                },
                [&](platform::WindowEventData<platform::WindowEventType::MouseInput> const& data) {

                });

            std::visit(event_visitor, event.data);
        });
    }
} // namespace ionengine