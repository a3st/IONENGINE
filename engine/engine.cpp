// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include "renderer/shader.hpp"

namespace ionengine
{
    Engine::Engine(platform::Window* window) : window(window), device(window), renderer(device)
    {
    }

    auto Engine::tick() -> void
    {
        auto startUpdateTime = std::chrono::high_resolution_clock::now();
        float deltaTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(startUpdateTime - lastUpdateTime).count() / 1000.0f;
        lastUpdateTime = startUpdateTime;

        this->update(deltaTime);

        device.beginFrame();
        renderer.swapchainTexture = device.getBackBuffer();

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

            lastUpdateTime = std::chrono::high_resolution_clock::now();

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
} // namespace ionengine