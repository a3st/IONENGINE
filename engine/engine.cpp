// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "precompiled.h"

namespace ionengine
{
    Engine::Engine(uint32_t argc, char** argv)
    {
        application = platform::App::create("Engine");
        window = std::make_unique<Window>(application);

        RHI = rhi::RHI::create(rhi::RHICreateInfo::Default());
        graphics = std::make_unique<Graphics>(RHI);
        gui = std::make_unique<GUI>(RHI);

        rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = application->getWindowHandle(),
                                                           .instance = application->getInstanceHandle()};
        auto swapchain = RHI->tryGetSwapchain(swapchainCreateInfo);

        application->windowUpdated += [this, swapchain]() -> void {
            auto endFrameTime = std::chrono::high_resolution_clock::now();
            float deltaTime =
                std::chrono::duration_cast<std::chrono::microseconds>(endFrameTime - beginFrameTime).count() /
                1000000.0f;

            this->onUpdate(deltaTime);

            auto swapchainTexture = swapchain->requestBackBuffer();

            graphics->beginFrame(swapchainTexture.get());
            this->onRender();
            gui->onRender();
            graphics->endFrame();

            swapchain->presentBackBuffer();

            beginFrameTime = endFrameTime;
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