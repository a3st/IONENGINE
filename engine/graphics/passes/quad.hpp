// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/graphics/render_pass.hpp"

namespace ionengine::passes
{
    class QuadPass : public RenderPass
    {
      public:
        QuadPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture,
                 core::ref_ptr<Shader> shader)
            : RenderPass("Quad Pass")
        {
            /*rhi::TextureCreateInfo const textureCreateInfo{
                .width = 800,
                .height = 600,
                .depth = 1,
                .mipLevels = 1,
                .format = rhi::TextureFormat::RGBA8_UNORM,
                .dimension = rhi::TextureDimension::_2D,
                .flags = (rhi::TextureUsageFlags)(rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource)};
            auto baseTexture = textureAllocator->allocate(textureCreateInfo);*/

            rhi::RenderPassColorInfo const renderPassColorInfo{.texture = cameraTexture,
                                                               .loadOp = rhi::RenderPassLoadOp::Clear,
                                                               .storeOp = rhi::RenderPassStoreOp::Store,
                                                               .clearColor = math::Color(0.5f, 0.5f, 0.5f, 1.0f)};
            this->colors.emplace_back(std::move(renderPassColorInfo));
            this->shader = shader;

            this->initializeRenderPass();
        }

        auto execute(rhi::GraphicsContext* context) -> void override
        {
            context->draw(3, 1);
        }
    };
} // namespace ionengine::passes