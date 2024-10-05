// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "platform/platform.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(RHI, DeviceOffRender_Test)
{
    rhi::RHICreateInfo rhiCreateInfo{.window = nullptr};
    auto device = rhi::Device::create(rhiCreateInfo);
}

class TestAppContext : public platform::AppContext
{
  public:
    TestAppContext() = default;

    auto initialize(platform::App& app)
    {
        rhi::RHICreateInfo rhiCreateInfo{.window = app.getWindowHandle(),
                                         .instance = app.getInstanceHandle()};
        this->device = rhi::Device::create(rhiCreateInfo);

        this->graphicsContext = device->createGraphicsContext();
    }

    auto onIdleEvent() -> void override
    {
        graphicsContext->reset();

        auto backBuffer = device->requestBackBuffer();

        std::vector<rhi::RenderPassColorInfo> colors{rhi::RenderPassColorInfo{.texture = backBuffer,
                                                                              .loadOp = rhi::RenderPassLoadOp::Clear,
                                                                              .storeOp = rhi::RenderPassStoreOp::Store,
                                                                              .clearColor = {0.5f, 0.6f, 0.7f, 1.0f}}};

        graphicsContext->setViewport(0, 0, width, height);
        graphicsContext->setScissor(0, 0, width, height);

        graphicsContext->barrier(backBuffer, rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
        graphicsContext->beginRenderPass(colors, std::nullopt);
        graphicsContext->endRenderPass();
        graphicsContext->barrier(backBuffer, rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);

        auto result = graphicsContext->execute();

        device->presentBackBuffer();

        result.wait();
    }

    auto onWindowEvent(platform::WindowEvent const& event) -> void override
    {
        switch (event.eventType)
        {
            case platform::WindowEventType::Resize: {
                width = event.size.width;
                height = event.size.height;

                if (device)
                {
                    device->resizeBackBuffers(width, height);
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    auto onInputEvent(platform::InputEvent const& event) -> void override
    {
        if (event.deviceType == platform::InputDeviceType::Keyboard)
        {
            if (event.state == platform::InputState::Pressed)
            {
                std::cout << "Key pressed: " << static_cast<uint32_t>(event.keyCode) << std::endl;
            }
            else
            {
                std::cout << "Key released: " << static_cast<uint32_t>(event.keyCode) << std::endl;
            }
        }
    }

  private:
    core::ref_ptr<rhi::Device> device;
    core::ref_ptr<rhi::GraphicsContext> graphicsContext;

    uint32_t width;
    uint32_t height;
};

TEST(RHI, DeviceSwapchain_Test)
{
    TestAppContext context;
    auto platform = platform::App::create(context, "TestProject");
    context.initialize(*platform);
    platform->run();
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}