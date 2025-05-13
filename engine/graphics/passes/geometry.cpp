// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "geometry.hpp"
#include "precompiled.h"

namespace ionengine::passes
{
    GeometryPass::GeometryPass(TextureAllocator& textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture)
        : RenderPass("Geometry Pass"), cameraTexture(cameraTexture)
    {
        rhi::RenderPassColorInfo const renderPassColorInfo{.texture = cameraTexture,
                                                           .loadOp = rhi::RenderPassLoadOp::Clear,
                                                           .storeOp = rhi::RenderPassStoreOp::Store,
                                                           .clearColor = core::Color(0.5f, 0.5f, 0.5f, 1.0f)};
        this->colors.emplace_back(std::move(renderPassColorInfo));

        this->initializeRenderPass();
    }

    auto GeometryPass::execute(RenderContext const& context) -> void
    {
        context.graphics->setViewport(0, 0, cameraTexture->getWidth(), cameraTexture->getHeight());
        context.graphics->setScissor(0, 0, cameraTexture->getWidth(), cameraTexture->getHeight());

        for (auto const& drawableData : renderGroups->at(RenderGroup::Opaque))
        {
            context.graphics->setGraphicsPipelineOptions(drawableData.shader->getShader(),
                                                         drawableData.shader->getRasterizerStageInfo(),
                                                         drawableData.shader->getBlendColorInfo(), std::nullopt);

            uint32_t const transformDataIndex =
                drawableData.shader->getBindings().at("SHADER_DATA").elements.at("gTransformData") / sizeof(uint32_t);
            context.graphics->bindDescriptor(transformDataIndex, drawableData.transformDataBuffer->getDescriptorOffset(
                                                                     rhi::BufferUsage::ConstantBuffer));

            uint32_t const effectDataIndex =
                drawableData.shader->getBindings().at("SHADER_DATA").elements.at("gEffectData") / sizeof(uint32_t);
            context.graphics->bindDescriptor(
                effectDataIndex, drawableData.effectDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));

            uint32_t const samplerDataIndex =
                drawableData.shader->getBindings().at("SHADER_DATA").elements.at("gSamplerData") / sizeof(uint32_t);
            context.graphics->bindDescriptor(
                samplerDataIndex, context.samplerDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));

            drawableData.surface->draw(context.graphics);
        }
    }
} // namespace ionengine::passes