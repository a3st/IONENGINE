// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "geometry.hpp"
#include "precompiled.h"

namespace ionengine::passes
{
    GeometryPass::GeometryPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture)
        : RenderPass("Geometry Pass")
    {
        rhi::RenderPassColorInfo const renderPassColorInfo{.texture = cameraTexture,
                                                           .loadOp = rhi::RenderPassLoadOp::Clear,
                                                           .storeOp = rhi::RenderPassStoreOp::Store,
                                                           .clearColor = core::Color(0.5f, 0.5f, 0.5f, 1.0f)};
        this->colors.emplace_back(std::move(renderPassColorInfo));

        this->initializeRenderPass();
    }

    auto GeometryPass::execute(RenderContext const& context, RenderableData const& renderableData) -> void
    {
        for (auto const& drawableData : renderableData.renderGroups.at(RenderGroup::Opaque))
        {
            context.graphics->setGraphicsPipelineOptions(drawableData.shader->getShader(),
                                                         drawableData.shader->getRasterizerStageInfo(),
                                                         drawableData.shader->getBlendColorInfo(), std::nullopt);

            uint32_t const transformDataIndex =
                drawableData.shader->getBindings().at("SHADER_DATA").elements.at("gTransformData") / sizeof(uint32_t);

            context.graphics->bindDescriptor(transformDataIndex, drawableData.transformDataBuffer->getDescriptorOffset(
                                                                     rhi::BufferUsage::ConstantBuffer));

            drawableData.surface.draw(context.graphics);
        }
    }
} // namespace ionengine::passes