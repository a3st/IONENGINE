// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "math/matrix.hpp"
#include "platform/platform.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"
#include "shadersys/compiler.hpp"
#include <gtest/gtest.h>
#include "mdl/obj/obj.hpp"

using namespace ionengine;

TEST(RHI, DeviceOffRender_Test)
{
    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());
}

auto FXVertexFormat_to_RHIVertexFormat(asset::fx::VertexFormat const format) -> rhi::VertexFormat
{
    switch (format)
    {
        case asset::fx::VertexFormat::RGBA32_FLOAT:
            return rhi::VertexFormat::RGBA32_FLOAT;
        case asset::fx::VertexFormat::RGBA32_SINT:
            return rhi::VertexFormat::RGBA32_SINT;
        case asset::fx::VertexFormat::RGBA32_UINT:
            return rhi::VertexFormat::RGBA32_UINT;
        case asset::fx::VertexFormat::RGB32_FLOAT:
            return rhi::VertexFormat::RGB32_FLOAT;
        case asset::fx::VertexFormat::RGB32_SINT:
            return rhi::VertexFormat::RGB32_SINT;
        case asset::fx::VertexFormat::RGB32_UINT:
            return rhi::VertexFormat::RGB32_UINT;
        case asset::fx::VertexFormat::RG32_FLOAT:
            return rhi::VertexFormat::RG32_FLOAT;
        case asset::fx::VertexFormat::RG32_SINT:
            return rhi::VertexFormat::RG32_SINT;
        case asset::fx::VertexFormat::RG32_UINT:
            return rhi::VertexFormat::RG32_UINT;
        case asset::fx::VertexFormat::R32_FLOAT:
            return rhi::VertexFormat::R32_FLOAT;
        case asset::fx::VertexFormat::R32_SINT:
            return rhi::VertexFormat::R32_SINT;
        case asset::fx::VertexFormat::R32_UINT:
            return rhi::VertexFormat::R32_UINT;
        default:
            throw std::invalid_argument("passed invalid argument into function");
    }
}

TEST(RHI, DeviceSwapchain_Test)
{
    auto application = platform::App::create("TestProject");

    uint32_t width;
    uint32_t height;

    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());

    auto graphicsContext = rhi->getGraphicsContext();
    auto copyContext = rhi->getCopyContext();

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

    // Compile shaders
    auto shaderCompiler = shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::DXIL);
    std::string errors;
    auto compileResult = shaderCompiler->compileFromFile("../../engine/shaders/base3d.fx", errors);
    ASSERT_TRUE(compileResult.has_value());

    // Load shader
    core::ref_ptr<rhi::Shader> shader;
    {
        asset::ShaderFile shaderFile = std::move(compileResult.value());

        auto const& vertexStage = shaderFile.shaderData.stages.at(asset::fx::StageType::Vertex);
        auto const& vertexBuffer = shaderFile.shaderData.buffers[vertexStage.buffer];

        std::vector<rhi::VertexDeclarationInfo> vertexDeclarations;
        for (auto& element : vertexStage.vertexLayout.value().elements)
        {
            rhi::VertexDeclarationInfo const vertexDeclaration{
                .semantic = element.semantic, .format = FXVertexFormat_to_RHIVertexFormat(element.format)};
            vertexDeclarations.emplace_back(vertexDeclaration);
        }

        auto const& pixelStage = shaderFile.shaderData.stages.at(asset::fx::StageType::Pixel);
        auto const& pixelBuffer = shaderFile.shaderData.buffers[pixelStage.buffer];

        rhi::ShaderCreateInfo const shaderCreateInfo{
            .pipelineType = rhi::PipelineType::Graphics,
            .graphics = {.vertexDeclarations = vertexDeclarations,
                         .vertexStage = {.entryPoint = vertexStage.entryPoint,
                                         .shaderCode = std::span<uint8_t const>(
                                             shaderFile.blob.data() + vertexBuffer.offset, vertexBuffer.size)},
                         .pixelStage = {.entryPoint = pixelStage.entryPoint,
                                        .shaderCode = std::span<uint8_t const>(
                                            shaderFile.blob.data() + pixelBuffer.offset, pixelBuffer.size)}}};
        shader = rhi->createShader(shaderCreateInfo);
    }

    rhi::BufferCreateInfo const bufferCreateInfo{.size = 1 * 1024 * 1024,
                                                 .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::Vertex};
    auto vertexBuffer = rhi->createBuffer(bufferCreateInfo);

    // Load model
    {
        auto objImporter = core::make_ref<asset::OBJImporter>();
        auto modelResult = objImporter->loadFromFile("box.obj", errors);
        ASSERT_TRUE(modelResult.has_value());

        asset::ModelFile modelFile = std::move(modelResult.value());
    }

    //copyContext->updateBuffer(vertexBuffer, 0, )

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

        std::vector<rhi::RenderPassColorInfo> colors{rhi::RenderPassColorInfo{.texture = backBuffer.get(),
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