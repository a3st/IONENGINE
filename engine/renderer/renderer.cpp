// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(rhi::RHICreateInfo const& createInfo, std::vector<core::ref_ptr<RenderPass>> const& renderPasses)
        : renderPasses(renderPasses)
    {
        device = rhi::Device::create(createInfo);

        graphicsContext = device->createGraphicsContext();
        copyContext = device->createCopyContext();

        constantBufferPool = core::make_ref<ConstantBufferPool>(*device);

        for (auto& renderPass : renderPasses)
        {
            renderPass->graphicsContext = graphicsContext.get();
            renderPass->copyContext = copyContext.get();
            renderPass->constantBufferPool = constantBufferPool.get();
            renderPass->isInitialized = false;
        }
    }

    auto Renderer::render(RenderingData& renderingData) -> void
    {
        graphicsContext->reset();

        renderingData.backBuffer = device->requestBackBuffer();

        outputWidth = renderingData.backBuffer->getWidth(), outputHeight = renderingData.backBuffer->getHeight();

        graphicsContext->setViewport(0, 0, outputWidth, outputHeight);
        graphicsContext->setScissor(0, 0, outputWidth, outputHeight);

        for (auto const& renderPass : renderPasses)
        {
            renderPass->render(renderingData);

            if (!renderPass->isInitialized)
            {
                renderPass->isInitialized = true;
            }
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

    auto Renderer::createShader(shadersys::ShaderFile const& shaderFile) -> core::ref_ptr<Shader>
    {
        return core::make_ref<Shader>(*device, shaderFile);
    }

    auto Renderer::createTexture() -> core::ref_ptr<Texture>
    {
        std::cout << "call ::createTexture" << std::endl;
        return nullptr;
    }

    auto Renderer::createModel(ModelFile const& modelFile) -> core::ref_ptr<Model>
    {
        return core::make_ref<Model>(*device, *copyContext, modelFile);
    }

    auto Renderer::createMaterial(MaterialDomain const domain, MaterialBlend const blend,
                                  core::ref_ptr<Shader> shader) -> core::ref_ptr<Material>
    {
        return core::make_ref<Material>(*device, domain, blend, shader);
    }

    auto RendererBuilder::build(rhi::RHICreateInfo const& createInfo) -> core::ref_ptr<Renderer>
    {
        return core::make_ref<Renderer>(createInfo, renderPasses);
    }
} // namespace ionengine