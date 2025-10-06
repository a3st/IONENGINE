// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "graphics/graphics.hpp"
#include "precompiled.h"
#include "window/window.hpp"

namespace ionengine
{
    auto EngineBuilder::withArgs(uint32_t argc, char** argv) -> EngineBuilder&
    {
        return *this;
    }

    auto EngineBuilder::withAppName(std::string_view const appName) -> EngineBuilder&
    {
        return *this;
    }

    auto EngineBuilder::build() -> core::ref_ptr<Engine>
    {
        return core::make_ref<Engine>();
    }

    auto Engine::getEnvironment() -> EngineEnvironment&
    {
        return _environment;
    }

    Engine::Engine()
    {
        _app = platform::App::create("Test");

        if (!_environment.registerModule<Window>(_app))
        {
            throw std::runtime_error("An error occurred while registering engine module");
        }

        if (!_environment.registerModule<Graphics>(_app))
        {
            throw std::runtime_error("An error occurred while registering engine module");
        }

        /*
        rhi::RHICreateInfo const rhiCreateInfo{.stagingBufferSize = 8 * 1024 * 1024, .numBuffering = 2};
        rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = application->getWindowHandle(),
                                                           .instance = application->getInstanceHandle()};
        RHI = rhi::RHI::create(rhiCreateInfo, swapchainCreateInfo);
        graphics = std::make_unique<Graphics>(RHI, rhiCreateInfo.numBuffering);
        gui = std::make_unique<GUI>(RHI);

        application->windowStateChanged += [this](platform::WindowEvent const& event) -> void {
            switch (event.type)
            {
                case platform::WindowEventType::Resize: {
                    uint32_t const width = event.size.width;
                    uint32_t const height = event.size.height;

                    if (graphics && width > 0 && height > 0)
                    {
                        graphics->onResize(width, height);
                    }

                    if (gui && width > 0 && height > 0)
                    {
                        gui->onResize(width, height);
                    }
                    break;
                }
            }
        };

        application->windowUpdated += [this]() -> void {
            auto endFrameTime = std::chrono::high_resolution_clock::now();
            float deltaTime =
                std::chrono::duration_cast<std::chrono::microseconds>(endFrameTime - beginFrameTime).count() /
                1000000.0f;

            gui->onUpdate();
            this->onUpdate(deltaTime);

            beginFrameTime = std::chrono::high_resolution_clock::now();

            graphics->beginFrame();
            this->onRender();
            gui->onRender();
            graphics->endFrame();
        };*/
    }

    auto Engine::run() -> int32_t
    {
        /*this->onStart();

        beginFrameTime = std::chrono::high_resolution_clock::now();

        Material::baseSurfaceMaterial = nullptr;*/
        
        _app->run();
        return EXIT_SUCCESS;
    }
} // namespace ionengine