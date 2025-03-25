// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/graphics/render_pass.hpp"

namespace ionengine::passes
{
    class GeometryPass : public RenderPass
    {
      public:
        GeometryPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture)
            : RenderPass("Geometry Pass")
        {
            rhi::RenderPassColorInfo const renderPassColorInfo{.texture = cameraTexture,
                                                               .loadOp = rhi::RenderPassLoadOp::Clear,
                                                               .storeOp = rhi::RenderPassStoreOp::Store,
                                                               .clearColor = core::Color(0.5f, 0.5f, 0.5f, 1.0f)};
            this->colors.emplace_back(std::move(renderPassColorInfo));

            this->initializeRenderPass();
        }

        auto execute(RenderContext const& context, RenderableData const& renderableData) -> void override
        {
            for (auto const& drawableData : renderableData.renderGroups.at(RenderGroup::Opaque))
            {
                context.graphics->setGraphicsPipelineOptions(drawableData.shader->getShader(),
                                                             drawableData.shader->getRasterizerStageInfo(),
                                                             drawableData.shader->getBlendColorInfo(), std::nullopt);

                core::weak_ptr<rhi::Buffer> transformDataBuffer;
                {
                    auto const& transformData = drawableData.shader->getBindings().at("TRANSFORM_DATA");

                    uint32_t const modelViewProjOffset =
                        drawableData.shader->getBindings().at("TRANSFORM_DATA").elements.at("modelViewProj");

                    std::vector<uint8_t> transformDataRawBuffer(transformData.size);

                    core::Mat4f modelViewProjMat =
                        drawableData.modelMat * renderableData.viewMat * renderableData.projMat;
                    std::memcpy(transformDataRawBuffer.data() + modelViewProjOffset, modelViewProjMat.data(),
                                sizeof(core::Mat4f));

                    transformDataBuffer = context.constBufferAllocator->allocate(transformData.size);

                    internal::UploadBufferInfo const uploadBufferInfo{
                        .buffer = transformDataBuffer.get(), .offset = 0, .dataBytes = transformDataRawBuffer};
                    context.uploadManager->uploadBuffer(uploadBufferInfo, []() -> void {});
                }

                uint32_t const transformDataIndex =
                    drawableData.shader->getBindings().at("SHADER_DATA").elements.at("gTransformData") /
                    sizeof(uint32_t);

                context.graphics->bindDescriptor(
                    transformDataIndex, transformDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));

                drawableData.surface.draw(context.graphics);
            }
        }
    };
} // namespace ionengine::passes