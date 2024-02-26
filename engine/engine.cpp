// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "core/exception.hpp"
#include "extensions/importers/glb.hpp"
#include "precompiled.h"

namespace ionengine
{
    Engine::Engine(std::filesystem::path const shader_path, core::ref_ptr<platform::Window> window)
        : window(window), job_system(core::make_ref<JobSystem>())
    {
        if (window)
        {
            render_context.device = rhi::Device::create(window.get());
        }
        else
        {
            render_context.device = rhi::Device::create(nullptr);
        }

        render_context.graphics_context = render_context.device->create_graphics_context();
        render_context.copy_context = render_context.device->create_copy_context();
    }

    auto Engine::tick() -> void
    {
        if (render_context.copy_task.get_result())
        {
            if (!render_context.streaming.empty())
            {
                render_context.copy_context->reset();
                
                auto asset = render_context.streaming.front();
                render_context.streaming.pop();

                utils::variant_match(asset)
                    .case_<StreamingData<Model>>([&](auto& data) {
                        render_context.copy_context->write_buffer(data.buffer, data.data);
                    });
            }
        }

        core::ref_ptr<rhi::Texture> back_buffer;

        if (window)
        {
            back_buffer = render_context.device->request_back_buffer();
        }

        render_context.graphics_context->reset();

        std::vector<rhi::RenderPassColorInfo> colors = {
            rhi::RenderPassColorInfo{.texture = back_buffer,
                                     .load_op = rhi::RenderPassLoadOp::Clear,
                                     .store_op = rhi::RenderPassStoreOp::Store,
                                     .clear_color = math::Color(0.2f, 0.3f, 0.5f, 1.0f)}};

        render_context.graphics_context->barrier(back_buffer, rhi::ResourceState::Common,
                                                 rhi::ResourceState::RenderTarget);
        render_context.graphics_context->begin_render_pass(colors, std::nullopt);
        render_context.graphics_context->end_render_pass();
        render_context.graphics_context->barrier(back_buffer, rhi::ResourceState::RenderTarget,
                                                 rhi::ResourceState::Common);

        rhi::Future<rhi::Query> result = render_context.graphics_context->execute();

        if (back_buffer)
        {
            render_context.device->present_back_buffer();
        }
        result.wait();
    }

    auto Engine::run() -> void
    {
        try
        {
            if (!window)
            {
                throw core::Exception("Window not found when creating an Engine instance");
            }

            bool is_running = true;
            platform::WindowEvent event;
            while (is_running)
            {
                while (window->try_get_event(event))
                {
                    utils::variant_match(event).case_<platform::WindowEventData<platform::WindowEventType::Closed>>(
                        [&](auto& data) { is_running = false; });
                }

                tick();
            }
        }
        catch (core::Exception e)
        {
            throw std::runtime_error(e.what());
        }
    }

    auto Engine::load_model(std::filesystem::path const& file_path) -> AssetFuture<Model>
    {
        auto model = core::make_ref<Model>(render_context);

        auto result = job_system->submit(
            [&, file_path]() {
                std::vector<uint8_t> data;
                {
                    std::basic_ifstream<uint8_t> ifs(file_path, std::ios::binary);
                    ifs.seekg(0, std::ios::end);
                    size_t const size = ifs.tellg();
                    ifs.seekg(0, std::ios::beg);
                    data.resize(size);
                    ifs.read(data.data(), data.size());
                }

                std::unique_ptr<ModelImporter> importer;

                if (file_path.extension().compare(".glb") == 0)
                {
                    importer = std::make_unique<GLBImporter>();
                }

                model->load_from_memory(data, *importer);
            },
            JobQueuePriority::Low);

        return AssetFuture<Model>(model, job_system, result);
    }

    auto Engine::load_texture(std::filesystem::path const& file_path) -> AssetFuture<Texture>
    {
        return AssetFuture<Texture>();
    }
} // namespace ionengine