// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics.hpp"
#include "precompiled.h"

namespace ionengine
{
    Graphics::Graphics(core::ref_ptr<platform::App> app, EngineEnvironment& environment) : _environment(environment)
    {
        assert(app && "core::ref_ptr<platform::App> is nullptr");
        _app = app;

        rhi::RHICreateInfo const rhiCreateInfo{.stagingBufferSize = 8 * 1024 * 1024, .numBuffering = 2};
        rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = app->getWindowHandle(),
                                                           .instance = app->getInstanceHandle()};
        _rhi = rhi::RHI::create(rhiCreateInfo, swapchainCreateInfo);
    }

    Graphics::Graphics(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options)
        : _environment(environment)
    {
        assert(app && "core::ref_ptr<platform::App> is nullptr");
        _app = app;
        _app->windowStateChanged += [this](platform::WindowEvent const& event) -> void {
            if (event.type == platform::WindowEventType::Resize)
            {
                this->onWindowResized(event);
            }
        };

        rhi::RHICreateInfo const rhiCreateInfo{.stagingBufferSize = options.stagingBufferSize, .numBuffering = 2};
        rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = app->getWindowHandle(),
                                                           .instance = app->getInstanceHandle()};
        _rhi = rhi::RHI::create(rhiCreateInfo, swapchainCreateInfo);
    }

    auto Graphics::onWindowResized(platform::WindowEvent const& event) -> void
    {
        if (!(event.size.width > 0 && event.size.height > 0))
        {
            return;
        }

        _rhi->getSwapchain()->resizeBackBuffers(event.size.width, event.size.height);
    }
} // namespace ionengine