// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "ui.hpp"
#include "precompiled.h"

namespace ionengine::passes
{
    UIPass::UIPass(TextureAllocator& textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture)
        : RenderPass("UI Pass"), cameraTexture(cameraTexture)
    {
        rhi::RenderPassColorInfo const renderPassColorInfo{
            .texture = cameraTexture, .loadOp = rhi::RenderPassLoadOp::Load, .storeOp = rhi::RenderPassStoreOp::Store};
        this->colors.emplace_back(std::move(renderPassColorInfo));

        this->initializeRenderPass();
    }

    auto UIPass::execute(RenderContext const& context, RenderableData const& renderableData) -> void
    {
        context.graphics->setViewport(0, 0, cameraTexture->getWidth(), cameraTexture->getHeight());
        context.graphics->setScissor(0, 0, cameraTexture->getWidth(), cameraTexture->getHeight());

        for (auto const& drawableData : renderableData.renderGroups.at(RenderGroup::UI))
        {
            context.graphics->setGraphicsPipelineOptions(drawableData.shader->getShader(),
                                                         drawableData.shader->getRasterizerStageInfo(),
                                                         rhi::BlendColorInfo::AlphaBlend(), std::nullopt);

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