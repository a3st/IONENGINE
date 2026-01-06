// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics.hpp"
#include "precompiled.h"

namespace ionengine
{
    Graphics::Graphics(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options)
        : _environment(environment), _shadersPath(options.shadersPath), frameIndex(0), _app(app)
    {
        _app->windowStateChanged += [this](platform::WindowEvent const& event) -> void {
            if (event.type == platform::WindowEventType::Resize)
            {
                this->onWindowResized(event);
            }
        };
        _app->windowUpdated += [this]() -> void { this->onWindowUpdated(); };

        rhi::RHICreateInfo const rhiCreateInfo{.stagingBufferSize = options.stagingBufferSize, .numBuffering = 2};
        rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = app->getWindowHandle(),
                                                           .instance = app->getInstanceHandle()};
        _rhi = rhi::RHI::create(rhiCreateInfo, swapchainCreateInfo);

        for (uint32_t const i : std::views::iota(0u, 2u))
        {
            FrameData frameData{.frameBufferPool = core::make_ref<TexturePool>(_rhi)};
            _frames.emplace_back(std::move(frameData));
        }

        _curGraphicsPipeline = options.graphicsPipeline;
    }

    Graphics::Graphics(core::ref_ptr<platform::App> app, EngineEnvironment& environment)
        : Graphics(app, environment, {})
    {
    }

    auto Graphics::onWindowResized(platform::WindowEvent const& event) -> void
    {
        if (!(event.size.width > 0 && event.size.height > 0))
        {
            return;
        }

        _rhi->getSwapchain()->resizeBackBuffers(event.size.width, event.size.height);
    }

    auto Graphics::onWindowUpdated() -> void
    {
        _curGraphicsPipeline->bindAttachment("Ext_Swapchain", _rhi->getSwapchain()->getBackBuffer());

        _curGraphicsPipeline->execute(*_rhi, *_frames[frameIndex].frameBufferPool);

        auto presentResult = _rhi->getSwapchain()->presentBackBuffer();

        presentResult.wait();
    }

    auto Graphics::initialize() -> void
    {
        // for (auto const& entry : std::filesystem::directory_iterator(_shadersPath))
        {
            // auto shader = core::make_ref<Shader>();
        }
    }
} // namespace ionengine