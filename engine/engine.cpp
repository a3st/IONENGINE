// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "core/exception.hpp"
#include "precompiled.h"

namespace ionengine
{
    Engine::Engine(std::filesystem::path const shader_path, core::ref_ptr<platform::Window> window) : window(window)
    {
        if (window)
        {
            device = rhi::Device::create(window.get());
        }
        else
        {
            device = rhi::Device::create(nullptr);
        }

        graphics_context = device->create_graphics_context();
    }

    auto Engine::tick() -> void
    {
        core::ref_ptr<rhi::Texture> back_buffer;

        if (window)
        {
            back_buffer = device->request_back_buffer();
        }

        graphics_context->reset();

        rhi::Future<rhi::Query> result = graphics_context->execute();

        if(back_buffer) {
            device->present_back_buffer();
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
} // namespace ionengine