// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine
{
    Engine::Engine(core::ref_ptr<platform::Window> window)
        : window(window), device(window), shader_manager(device), asset_loader(device), renderer(device)
    {
        init();
    }

    auto Engine::tick() -> void
    {
        update(0.0f);
        device.begin_frame();
        render();
        device.end_frame();
    }

    auto Engine::run() -> int32_t
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
        return EXIT_SUCCESS;
    }

    auto Engine::init() -> void
    {
    }

    auto Engine::update(float const dt) -> void
    {
    }

    auto Engine::render() -> void
    {
    }

    /*
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
    */
} // namespace ionengine