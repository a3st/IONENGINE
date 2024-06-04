// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include "renderer/shader.hpp"

namespace ionengine
{
    Engine::Engine(platform::Window* window) : window(window), device(window)
    {
    }

    auto Engine::tick() -> void
    {
        this->update(0.0f);
        device.beginFrame();
        this->render();
        device.endFrame();
    }

    auto Engine::loop() -> void
    {
        this->tick();
    }

    auto Engine::run() -> void
    {
        try
        {
            this->init();

            if (window)
            {
                bool isRunning = true;
                platform::WindowEvent event;
                while (isRunning)
                {
                    while (window->try_get_event(event))
                    {
                        utils::variant_match(event).case_<platform::WindowEventData<platform::WindowEventType::Closed>>(
                            [&](auto& data) { isRunning = false; });
                    }

                    this->tick();
                }
            }
        }
        catch (core::Exception e)
        {
            throw std::runtime_error(e.what());
        }
    }

    auto Engine::createShaderAsset() -> core::ref_ptr<ShaderAsset>
    {
        return core::make_ref<ShaderAsset>(device);
    }

    /*auto Engine::create_texture(uint32_t const width, uint32_t const height, TextureFlags const flags)
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
    }*/
} // namespace ionengine