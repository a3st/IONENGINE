// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "core/matrix.hpp"
#include "core/quaternion.hpp"
#include "mdl/obj/obj.hpp"
#include "platform/platform.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"
#include "shadersys/compiler.hpp"
#include "txe/cmp/cmp.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

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
            throw std::invalid_argument("unknown VertexFormat for passed argument");
    }
}

auto TXETextureFormat_to_RHITextureFormat(asset::txe::TextureFormat const format) -> rhi::TextureFormat
{
    switch (format)
    {
        case asset::txe::TextureFormat::RGBA8_UNORM:
            return rhi::TextureFormat::RGBA8_UNORM;
        default:
            throw std::invalid_argument("unknown TextureFormat for passed argument");
    }
}

TEST(RHI, DeviceOnly_Test)
{
    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());
}

TEST(RHI, RenderQuad_Test)
{
    auto application = platform::App::create("Quad");

    uint32_t width, height;

    // Create device and swapchain using RHI
    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());
    auto graphicsContext = rhi->getGraphicsContext();
    auto copyContext = rhi->getCopyContext();
    rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = application->getWindowHandle(),
                                                       .instance = application->getInstanceHandle()};
    auto swapchain = rhi->tryGetSwapchain(swapchainCreateInfo);

    // Compile shaders
    asset::fx::ShaderFormat shaderFormat;
#ifdef IONENGINE_RHI_DIRECTX12
    shaderFormat = asset::fx::ShaderFormat::DXIL;
#elif IONENGINE_RHI_VULKAN
    shaderFormat = asset::fx::ShaderFormat::SPIRV;
#endif
    auto shaderCompiler = shadersys::ShaderCompiler::create(shaderFormat);
    auto compileResult = shaderCompiler->compileFromFile("../../engine/shaders/quad.fx");
    if (!compileResult.has_value())
    {
        std::cerr << compileResult.error().what() << std::endl;
    }
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
            .shaderType = rhi::ShaderType::Graphics,
            .graphics = {.vertexDeclarations = vertexDeclarations,
                         .vertexStage = {.entryPoint = vertexStage.entryPoint,
                                         .shaderCode = std::span<uint8_t const>(
                                             shaderFile.blob.data() + vertexBuffer.offset, vertexBuffer.size)},
                         .pixelStage = {.entryPoint = pixelStage.entryPoint,
                                        .shaderCode = std::span<uint8_t const>(
                                            shaderFile.blob.data() + pixelBuffer.offset, pixelBuffer.size)}}};
        shader = rhi->createShader(shaderCreateInfo);
    }

    // Load texture
    core::ref_ptr<rhi::Texture> basicTexture;
    {
        auto stbiImporter = core::make_ref<asset::CMPImporter>(true);
        auto textureResult = stbiImporter->loadFromFile("../../assets/textures/spngbob.png");
        ASSERT_TRUE(textureResult.has_value());

        asset::TextureFile textureFile = std::move(textureResult.value());

        rhi::TextureCreateInfo const textureCreateInfo{
            .width = textureFile.textureData.width,
            .height = textureFile.textureData.height,
            .depth = 1,
            .mipLevels = static_cast<uint32_t>(textureFile.textureData.mipLevels.size()),
            .format = TXETextureFormat_to_RHITextureFormat(textureFile.textureData.format),
            .dimension = rhi::TextureDimension::_2D,
            .flags = (rhi::TextureUsageFlags)(rhi::TextureUsage::ShaderResource | rhi::TextureUsage::CopyDest)};
        basicTexture = rhi->createTexture(textureCreateInfo);

        copyContext->barrier(basicTexture, rhi::ResourceState::Common, rhi::ResourceState::CopyDest);

        for (uint32_t const i : std::views::iota(0u, textureFile.textureData.mipLevels.size()))
        {
            auto const& mipBuffer = textureFile.textureData.buffers[textureFile.textureData.mipLevels[i]];

            auto copyResult = copyContext->updateTexture(
                basicTexture, i, std::span<uint8_t const>(textureFile.blob.data() + mipBuffer.offset, mipBuffer.size));
        }

        copyContext->barrier(basicTexture, rhi::ResourceState::CopyDest, rhi::ResourceState::Common);

        auto executeResult = copyContext->execute();
        executeResult.wait();
    }

    core::ref_ptr<rhi::Sampler> linearSampler;
    {
        rhi::SamplerCreateInfo const samplerCreateInfo{.filter = rhi::Filter::Anisotropic,
                                                       .addressU = rhi::AddressMode::Wrap,
                                                       .addressV = rhi::AddressMode::Wrap,
                                                       .addressW = rhi::AddressMode::Wrap,
                                                       .compareOp = rhi::CompareOp::LessEqual,
                                                       .maxAnisotropy = 4};
        linearSampler = rhi->createSampler(samplerCreateInfo);
    }

    core::ref_ptr<rhi::Buffer> sBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 65536,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        sBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    core::ref_ptr<rhi::Buffer> eBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 65536,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        eBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    // Initialize static resources
    {
        struct SamplerData
        {
            uint32_t linearSampler;
        };
        SamplerData const samplerData{.linearSampler = linearSampler->getDescriptorOffset()};

        copyContext->updateBuffer(
            sBuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&samplerData), sizeof(samplerData)));

        struct EffectData
        {
            uint32_t basicTexture;
        };
        EffectData const effectData{.basicTexture =
                                        basicTexture->getDescriptorOffset(rhi::TextureUsage::ShaderResource)};

        copyContext->updateBuffer(
            eBuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&effectData), sizeof(effectData)));

        auto executeResult = copyContext->execute();
        executeResult.wait();

        graphicsContext->barrier(basicTexture, rhi::ResourceState::Common, rhi::ResourceState::ShaderRead);
    }

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

    std::vector<rhi::RenderPassColorInfo> colors;

    application->windowUpdated += [&]() -> void {
        auto backBuffer = swapchain->requestBackBuffer();

        colors.emplace_back(rhi::RenderPassColorInfo{.texture = backBuffer.get(),
                                                     .loadOp = rhi::RenderPassLoadOp::Clear,
                                                     .storeOp = rhi::RenderPassStoreOp::Store,
                                                     .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}});

        graphicsContext->setViewport(0, 0, width, height);
        graphicsContext->setScissor(0, 0, width, height);

        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
        graphicsContext->beginRenderPass(colors, std::nullopt);
        graphicsContext->setGraphicsPipelineOptions(
            shader, rhi::RasterizerStageInfo{.fillMode = rhi::FillMode::Solid, .cullMode = rhi::CullMode::Back},
            rhi::BlendColorInfo::Opaque(), std::nullopt);
        graphicsContext->bindDescriptor(0, sBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
        graphicsContext->bindDescriptor(1, eBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
        graphicsContext->draw(3, 1);
        graphicsContext->endRenderPass();
        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);

        colors.clear();

        auto executeResult = graphicsContext->execute();
        executeResult.wait();

        swapchain->presentBackBuffer();
    };

    application->run();
}

TEST(RHI, RenderModel_Test)
{
    auto application = platform::App::create("Cube");

    uint32_t width, height;

    // Create device and swapchain using RHI
    auto rhi = rhi::RHI::create(rhi::RHICreateInfo::Default());
    auto graphicsContext = rhi->getGraphicsContext();
    auto copyContext = rhi->getCopyContext();
    rhi::SwapchainCreateInfo const swapchainCreateInfo{.window = application->getWindowHandle(),
                                                       .instance = application->getInstanceHandle()};
    auto swapchain = rhi->tryGetSwapchain(swapchainCreateInfo);

    // Compile shaders
    asset::fx::ShaderFormat shaderFormat;
#ifdef IONENGINE_RHI_DIRECTX12
    shaderFormat = asset::fx::ShaderFormat::DXIL;
#elif IONENGINE_RHI_VULKAN
    shaderFormat = asset::fx::ShaderFormat::SPIRV;
#endif
    auto shaderCompiler = shadersys::ShaderCompiler::create(shaderFormat);
    auto compileResult = shaderCompiler->compileFromFile("../../engine/shaders/base3d.fx");
    if (!compileResult.has_value())
    {
        std::cerr << compileResult.error().what() << std::endl;
    }
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
            vertexDeclarations.emplace_back(std::move(vertexDeclaration));
        }

        auto const& pixelStage = shaderFile.shaderData.stages.at(asset::fx::StageType::Pixel);
        auto const& pixelBuffer = shaderFile.shaderData.buffers[pixelStage.buffer];

        rhi::ShaderCreateInfo const shaderCreateInfo{
            .shaderType = rhi::ShaderType::Graphics,
            .graphics = {.vertexDeclarations = vertexDeclarations,
                         .vertexStage = {.entryPoint = vertexStage.entryPoint,
                                         .shaderCode = std::span<uint8_t const>(
                                             shaderFile.blob.data() + vertexBuffer.offset, vertexBuffer.size)},
                         .pixelStage = {.entryPoint = pixelStage.entryPoint,
                                        .shaderCode = std::span<uint8_t const>(
                                            shaderFile.blob.data() + pixelBuffer.offset, pixelBuffer.size)}}};
        shader = rhi->createShader(shaderCreateInfo);
    }

    // Allocate resources
    core::ref_ptr<rhi::Buffer> vBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 1 * 1024 * 1024,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::Vertex};
        vBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    core::ref_ptr<rhi::Buffer> iBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 1 * 1024 * 1024,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::Index};
        iBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    core::ref_ptr<rhi::Buffer> tBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 65536,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        tBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    core::ref_ptr<rhi::Buffer> sBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 65536,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        sBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    core::ref_ptr<rhi::Buffer> eBuffer;
    {
        rhi::BufferCreateInfo const bufferCreateInfo{.size = 65536,
                                                     .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        eBuffer = rhi->createBuffer(bufferCreateInfo);
    }

    core::ref_ptr<rhi::Sampler> linearSampler;
    {
        rhi::SamplerCreateInfo const samplerCreateInfo{.filter = rhi::Filter::Anisotropic,
                                                       .addressU = rhi::AddressMode::Wrap,
                                                       .addressV = rhi::AddressMode::Wrap,
                                                       .addressW = rhi::AddressMode::Wrap,
                                                       .compareOp = rhi::CompareOp::LessEqual,
                                                       .maxAnisotropy = 4};
        linearSampler = rhi->createSampler(samplerCreateInfo);
    }

    uint32_t indexCount = 0;
    // Load model
    {
        auto objImporter = core::make_ref<asset::OBJImporter>();
        auto modelResult = objImporter->loadFromFile("../../assets/objects/box.obj");
        ASSERT_TRUE(modelResult.has_value());

        asset::ModelFile modelFile = std::move(modelResult.value());

        auto const& vertexBuffer = modelFile.modelData.buffers[modelFile.modelData.buffer];

        copyContext->barrier(vBuffer, rhi::ResourceState::Common, rhi::ResourceState::CopyDest);

        auto copyResult = copyContext->updateBuffer(
            vBuffer, 0, std::span<uint8_t const>(modelFile.blob.data() + vertexBuffer.offset, vertexBuffer.size));

        copyContext->barrier(vBuffer, rhi::ResourceState::CopyDest, rhi::ResourceState::Common);

        auto const& indexBuffer = modelFile.modelData.buffers[modelFile.modelData.surfaces[0].buffer];

        copyContext->barrier(iBuffer, rhi::ResourceState::Common, rhi::ResourceState::CopyDest);

        copyResult = copyContext->updateBuffer(
            iBuffer, 0, std::span<uint8_t const>(modelFile.blob.data() + indexBuffer.offset, indexBuffer.size));

        copyContext->barrier(iBuffer, rhi::ResourceState::CopyDest, rhi::ResourceState::Common);

        auto executeResult = copyContext->execute();
        executeResult.wait();

        indexCount = modelFile.modelData.surfaces[0].indexCount;
    }

    // Load texture
    core::ref_ptr<rhi::Texture> basicTexture;
    {
        auto stbiImporter = core::make_ref<asset::CMPImporter>(true);
        auto textureResult = stbiImporter->loadFromFile("../../assets/textures/debug-empty.png");
        ASSERT_TRUE(textureResult.has_value());

        asset::TextureFile textureFile = std::move(textureResult.value());

        rhi::TextureCreateInfo const textureCreateInfo{
            .width = textureFile.textureData.width,
            .height = textureFile.textureData.height,
            .depth = 1,
            .mipLevels = static_cast<uint32_t>(textureFile.textureData.mipLevels.size()),
            .format = TXETextureFormat_to_RHITextureFormat(textureFile.textureData.format),
            .dimension = rhi::TextureDimension::_2D,
            .flags = (rhi::TextureUsageFlags)(rhi::TextureUsage::ShaderResource | rhi::TextureUsage::CopyDest)};
        basicTexture = rhi->createTexture(textureCreateInfo);

        copyContext->barrier(basicTexture, rhi::ResourceState::Common, rhi::ResourceState::CopyDest);

        for (uint32_t const i : std::views::iota(0u, textureFile.textureData.mipLevels.size()))
        {
            auto const& mipBuffer = textureFile.textureData.buffers[textureFile.textureData.mipLevels[i]];

            auto copyResult = copyContext->updateTexture(
                basicTexture, i, std::span<uint8_t const>(textureFile.blob.data() + mipBuffer.offset, mipBuffer.size));
        }

        copyContext->barrier(basicTexture, rhi::ResourceState::CopyDest, rhi::ResourceState::Common);

        auto executeResult = copyContext->execute();
        executeResult.wait();
    }

    // Initialize static resources
    {
        struct SamplerData
        {
            uint32_t linearSampler;
        };
        SamplerData const samplerData{.linearSampler = linearSampler->getDescriptorOffset()};

        copyContext->updateBuffer(
            sBuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&samplerData), sizeof(samplerData)));

        struct EffectData
        {
            uint32_t basicTexture;
        };
        EffectData const effectData{.basicTexture =
                                        basicTexture->getDescriptorOffset(rhi::TextureUsage::ShaderResource)};

        copyContext->updateBuffer(
            eBuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&effectData), sizeof(effectData)));

        auto executeResult = copyContext->execute();
        executeResult.wait();

        graphicsContext->barrier(basicTexture, rhi::ResourceState::Common, rhi::ResourceState::ShaderRead);
    }

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

    core::Quatf originalRot = core::Quatf::euler(0.0f, 0.0f, 0.0f);
    float angle = 0.0f;
    auto beginFrameTime = std::chrono::high_resolution_clock::now();
    std::vector<rhi::RenderPassColorInfo> colors;

    application->windowUpdated += [&]() -> void {
        auto endFrameTime = std::chrono::high_resolution_clock::now();
        float deltaTime =
            std::chrono::duration_cast<std::chrono::microseconds>(endFrameTime - beginFrameTime).count() / 1000000.0f;

        // Update
        core::Mat4f projection =
            core::Mat4f::perspectiveRH(60.0f * std::numbers::pi / 180.0f, width / height, 0.1f, 100.0f);
        core::Mat4f view = core::Mat4f::lookAtRH(core::Vec3f(4.0f, 2.0f, 4.0f), core::Vec3f(0.0f, 0.0f, 0.0f),
                                                 core::Vec3f(0.0f, 0.0f, 1.0f));
        core::Quatf currentRot = originalRot * core::Quatf::fromAngleAxis(angle, core::Vec3f(0.0f, 0.0f, 1.0f));
        core::Mat4f model = core::Mat4f::identity() * currentRot.toMat();
        angle += 100.0f * deltaTime;

        auto backBuffer = swapchain->requestBackBuffer();

        // Update MVP matrix
        {
            struct TransformData
            {
                core::Mat4f modelViewProj;
            };
            TransformData const transformData{.modelViewProj = model * view * projection};

            copyContext->updateBuffer(
                tBuffer, 0,
                std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&transformData), sizeof(transformData)));

            auto executeResult = copyContext->execute();
            executeResult.wait();
        }

        colors.emplace_back(rhi::RenderPassColorInfo{.texture = backBuffer.get(),
                                                     .loadOp = rhi::RenderPassLoadOp::Clear,
                                                     .storeOp = rhi::RenderPassStoreOp::Store,
                                                     .clearColor = {0.5f, 0.6f, 0.7f, 1.0f}});

        graphicsContext->setViewport(0, 0, width, height);
        graphicsContext->setScissor(0, 0, width, height);

        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
        graphicsContext->beginRenderPass(colors, std::nullopt);
        graphicsContext->setGraphicsPipelineOptions(
            shader, rhi::RasterizerStageInfo{.fillMode = rhi::FillMode::Solid, .cullMode = rhi::CullMode::Front},
            rhi::BlendColorInfo::Opaque(), std::nullopt);
        graphicsContext->bindDescriptor(0, tBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
        graphicsContext->bindDescriptor(1, sBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
        graphicsContext->bindDescriptor(2, eBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
        graphicsContext->bindVertexBuffer(vBuffer, 0, vBuffer->getSize());
        graphicsContext->bindIndexBuffer(iBuffer, 0, iBuffer->getSize(), rhi::IndexFormat::Uint32);
        graphicsContext->drawIndexed(indexCount, 1);
        graphicsContext->endRenderPass();
        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);

        colors.clear();

        auto executeResult = graphicsContext->execute();
        executeResult.wait();

        swapchain->presentBackBuffer();

        beginFrameTime = endFrameTime;
    };

    application->run();
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}