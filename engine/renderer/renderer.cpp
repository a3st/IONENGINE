// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(rhi::Device& device, std::vector<core::ref_ptr<RenderPass>> const& renderPasses)
        : device(&device), renderPasses(renderPasses)
    {
        graphicsContext = device.createGraphicsContext();
        copyContext = device.createCopyContext();

        for (auto& renderPass : renderPasses)
        {
            renderPass->graphicsContext = graphicsContext.get();
            renderPass->copyContext = copyContext.get();
        }
    }

    auto Renderer::render() -> void
    {
        graphicsContext->reset();

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

    auto createShader(shadersys::fx::ShaderEffectFile const& shaderEffect) -> core::ref_ptr<Shader>
    {
        return nullptr;
    }

    auto RendererBuilder::build(rhi::Device& device) -> core::ref_ptr<Renderer>
    {
        return core::make_ref<Renderer>(device, renderPasses);
    }
} // namespace ionengine