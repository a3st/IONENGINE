// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "core/exception.hpp"
#include "extensions/importers/glb.hpp"
#include "libpng16/png.h"
#include "precompiled.h"

#include "material.hpp"

namespace ionengine
{
    Engine::Engine(std::filesystem::path const shader_path, core::ref_ptr<platform::Window> window)
        : window(window), device({}, window.get())
    {
        auto material = core::make_ref<Material>(device);
        material->create_using_shader("basic");
        material->set_param<float>("Opacity", 2.0f);
    }

    auto Engine::tick() -> void
    {
        /*device->begin_frame();

        std::vector<rhi::RenderPassColorInfo> colors = {
            rhi::RenderPassColorInfo{.texture = device->get_back_buffer(),
                                     .load_op = rhi::RenderPassLoadOp::Clear,
                                     .store_op = rhi::RenderPassStoreOp::Store,
                                     .clear_color = math::Color(0.2f, 0.3f, 0.5f, 1.0f)}};

        device->get_graphics_context().barrier(device->get_back_buffer(), rhi::ResourceState::Common,
                                               rhi::ResourceState::RenderTarget);
        device->get_graphics_context().begin_render_pass(colors, std::nullopt);
        device->get_graphics_context().end_render_pass();
        device->get_graphics_context().barrier(device->get_back_buffer(), rhi::ResourceState::RenderTarget,
                                               rhi::ResourceState::Common);

        device->end_frame();

        std::vector<std::vector<uint8_t>> data;

        device->begin_upload();
        device->read(device->get_back_buffer(), data);
        device->end_upload();

        std::vector<uint8_t*> image(device->get_back_buffer()->get_height());

        for (uint32_t const i : std::views::iota(0u, image.size()))
        {
            image[i] = data[0].data() + i * device->get_back_buffer()->get_width() * 4;
        }

        FILE* fp = fopen("test.png", "wb");
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop png_info = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, fp);
        png_set_IHDR(png_ptr, png_info, device->get_back_buffer()->get_width(), device->get_back_buffer()->get_height(),
                     8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_set_rows(png_ptr, png_info, &image[0]);
        png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);
        png_write_end(png_ptr, png_info);
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);

        std::cout << "Successful" << std::endl;

        std::exit(EXIT_SUCCESS);*/
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

    auto Engine::load_model(std::filesystem::path const& file_path) -> std::tuple<JobFuture, core::ref_ptr<Model>>
    {
        auto model = core::make_ref<Model>(device);

        JobFuture result = job_system.submit(
            [&, model, file_path]() {
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

        return std::make_tuple(std::move(result), model);
    }

    auto Engine::load_texture(std::filesystem::path const& file_path) -> std::tuple<JobFuture, core::ref_ptr<Texture>>
    {
        return {};
    }
} // namespace ionengine