// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "platform/platform.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"
#include <gtest/gtest.h>
#include "shadersys/compiler.hpp"
#include "math/matrix.hpp"

using namespace ionengine;

TEST(RHI, DeviceOffRender_Test)
{
    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());
}

TEST(RHI, DeviceSwapchain_Test)
{
    auto application = platform::App::create("TestProject");

    uint32_t width;
    uint32_t height;

    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());

    auto graphicsContext = rhi->getGraphicsContext();

    rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = application->getWindowHandle(),
                                                       .instance = application->getInstanceHandle()};
    auto swapchain = rhi->tryGetSwapchain(swapchainCreateInfo);

    application->inputStateChanged += [&](platform::InputEvent const& event) -> void {
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
    };

    // Compile Shaders
    auto shaderCompiler = shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::DXIL);
    std::string errors;
    auto compileResult = shaderCompiler->compileFromFile("../../shaders/base3d.fx", errors);
    ASSERT_TRUE(compileResult.has_value());

    

    application->windowStateChanged += [&](platform::WindowEvent const& event) -> void {
        switch (event.eventType)
        {
            case platform::WindowEventType::Resize: {
                width = event.size.width;
                height = event.size.height;

                if (rhi)
                {
                    swapchain->resizeBackBuffers(width, height);
                }
                break;
            }
            default: {
                break;
            }
        }
    };

    application->windowUpdated += [&]() -> void {
        auto backBuffer = swapchain->requestBackBuffer();

        std::vector<rhi::RenderPassColorInfo> colors{
            rhi::RenderPassColorInfo{.texture = backBuffer.get(),
                                     .loadOp = rhi::RenderPassLoadOp::Clear,
                                     .storeOp = rhi::RenderPassStoreOp::Store,
                                     .clearColor = {0.5f, 0.6f, 0.7f, 1.0f}}};

        graphicsContext->setViewport(0, 0, width, height);
        graphicsContext->setScissor(0, 0, width, height);

        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
        graphicsContext->beginRenderPass(colors, std::nullopt);
        graphicsContext->endRenderPass();
        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);

        auto executeResult = graphicsContext->execute();
        executeResult.wait();

        swapchain->presentBackBuffer();
    };

    application->run();
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}