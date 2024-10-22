// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(rhi::RHICreateInfo const& createInfo)
    {
        device = rhi::Device::create(createInfo);

        graphicsContext = device->createGraphicsContext();
        copyContext = device->createCopyContext();

        constantBufferPool = core::make_ref<ConstantBufferPool>(*device);
    }

    auto Renderer::render() -> void
    {
        graphicsContext->reset();

        auto backBuffer = device->requestBackBuffer();

        outputWidth = backBuffer->getWidth(), outputHeight = backBuffer->getHeight();

        graphicsContext->setViewport(0, 0, outputWidth, outputHeight);
        graphicsContext->setScissor(0, 0, outputWidth, outputHeight);

        graphicsContext->barrier(backBuffer.get(), rhi::ResourceState::Common,
                                 rhi::ResourceState::RenderTarget);

        std::vector<rhi::RenderPassColorInfo> colors{rhi::RenderPassColorInfo{.texture = backBuffer.get(),
                                                                              .loadOp = rhi::RenderPassLoadOp::Clear,
                                                                              .storeOp = rhi::RenderPassStoreOp::Store,
                                                                              .clearColor = {0.5f, 0.6f, 0.7f, 1.0f}}};
        graphicsContext->beginRenderPass(colors, std::nullopt);

        

        graphicsContext->endRenderPass();

        rhi::Future<rhi::Query> graphicsResult = graphicsContext->execute();

        device->presentBackBuffer();

        graphicsResult.wait();
    }

    auto Renderer::resize(uint32_t const width, uint32_t const height) -> void
    {
        device->resizeBackBuffers(width, height);

        outputWidth = width, outputHeight = height;
    }

    auto Renderer::createShader(shadersys::ShaderFile const& shaderFile) -> core::ref_ptr<Shader>
    {
        return core::make_ref<Shader>(*device, shaderFile);
    }

    auto Renderer::createTexture() -> core::ref_ptr<Texture>
    {
        std::cout << "call ::createTexture" << std::endl;
        return nullptr;
    }

    auto Renderer::createModel(asset::ModelFile const& modelFile) -> core::ref_ptr<Model>
    {
        return core::make_ref<Model>(*device, *copyContext, modelFile);
    }

    auto Renderer::createMaterial(core::ref_ptr<Shader> shader) -> core::ref_ptr<Material>
    {
        return core::make_ref<Material>(*device, shader);
    }
} // namespace ionengine