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
            init();

            if (window)
            {
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
                return EXIT_SUCCESS;
            }
            else
            {
                return 0;
            }
        }
        catch (core::Exception e)
        {
            throw std::runtime_error(e.what());
        }
    }

    auto Engine::create_texture(uint32_t const width, uint32_t const height, TextureFlags const flags)
        -> core::ref_ptr<Texture>
    {
        auto texture = core::make_ref<Texture>(device);
        texture->create(width, height, flags);
        return texture;
    }

    auto Engine::create_material(std::string_view const shader_name) -> core::ref_ptr<Material>
    {
        auto material = core::make_ref<Material>(device, shader_manager);
        material->create_using_shader(shader_name);
        return material;
    }
} // namespace ionengine