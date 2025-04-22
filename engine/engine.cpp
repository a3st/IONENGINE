// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "precompiled.h"

namespace ionengine
{
    Engine::Engine(uint32_t argc, char** argv)
    {
        application = platform::App::create("Engine");
        window = std::make_unique<Window>(application);

        rhi::RHICreateInfo const rhiCreateInfo{.stagingBufferSize = 8 * 1024 * 1024, .numBuffering = 2};
        rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = application->getWindowHandle(),
                                                           .instance = application->getInstanceHandle()};
        RHI = rhi::RHI::create(rhiCreateInfo, swapchainCreateInfo);
        graphics = std::make_unique<Graphics>(RHI, rhiCreateInfo.numBuffering);
        gui = std::make_unique<GUI>(RHI);

        application->windowStateChanged += [this](platform::WindowEvent const& event) -> void {
            switch (event.eventType)
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
        };
    }

    auto Engine::run() -> int32_t
    {
        this->onStart();

        beginFrameTime = std::chrono::high_resolution_clock::now();

        application->run();

        Material::baseSurfaceMaterial = nullptr;
        return EXIT_SUCCESS;
    }
} // namespace ionengine