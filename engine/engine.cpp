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

        application->windowUpdated += [this]() -> void {
            auto endFrameTime = std::chrono::high_resolution_clock::now();
            float deltaTime =
                std::chrono::duration_cast<std::chrono::microseconds>(endFrameTime - beginFrameTime).count() /
                1000000.0f;

            this->onUpdate(deltaTime);

            graphics->beginFrame();
            this->onRender();
            graphics->endFrame();

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