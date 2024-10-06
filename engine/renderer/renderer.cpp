// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(rhi::Device& device, std::vector<core::ref_ptr<RenderPass>> const& renderPasses,
                       core::ref_ptr<RenderPassStorage> passStorage, uint32_t const outputWidth,
                       uint32_t const outputHeight)
        : device(&device), renderPasses(renderPasses), passStorage(passStorage), outputWidth(outputWidth),
          outputHeight(outputHeight)
    {
        graphicsContext = device.createGraphicsContext();
        copyContext = device.createCopyContext();

        for (auto& renderPass : renderPasses)
        {
            renderPass->graphicsContext = graphicsContext.get();
            renderPass->copyContext = copyContext.get();
            renderPass->passStorage = passStorage.get();

            renderPass->initialize(this);
        }
    }

    auto Renderer::render() -> void
    {
        graphicsContext->reset();

        passStorage->backBuffer = device->requestBackBuffer();

        graphicsContext->setViewport(0, 0, outputWidth, outputHeight);
        graphicsContext->setScissor(0, 0, outputWidth, outputHeight);

        for (auto const& renderPass : renderPasses)
        {
            renderPass->render();
        }

        rhi::Future<rhi::Query> graphicsResult = graphicsContext->execute();

        device->presentBackBuffer();

        graphicsResult.wait();
    }

    auto Renderer::resize(uint32_t const width, uint32_t const height) -> void
    {
        device->resizeBackBuffers(width, height);

        outputWidth = width, outputHeight = height;
    }

    auto Renderer::createShader(shadersys::ShaderEffectFile const& shaderEffect) -> core::ref_ptr<Shader>
    {
        return core::make_ref<Shader>(*device, shaderEffect);
    }

    auto Renderer::createTexture() -> core::ref_ptr<Texture>
    {
        std::cout << "call ::createTexture" << std::endl;
        return nullptr;
    }

    auto RendererBuilder::build(rhi::Device& device, uint32_t const outputWidth,
                                uint32_t const outputHeight) -> core::ref_ptr<Renderer>
    {
        passStorage = core::make_ref<RenderPassStorage>();

        return core::make_ref<Renderer>(device, renderPasses, passStorage, outputWidth, outputHeight);
    }
} // namespace ionengine