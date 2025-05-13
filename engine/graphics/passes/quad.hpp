// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../render_pass.hpp"
#include "../texture_allocator.hpp"

namespace ionengine::passes
{
    class QuadPass : public RenderPass
    {
      public:
        QuadPass(TextureAllocator& textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture,
                 core::ref_ptr<Shader> shader)
            : RenderPass("Quad Pass"), shader(shader)
        {
            rhi::RenderPassColorInfo const renderPassColorInfo{.texture = cameraTexture,
                                                               .loadOp = rhi::RenderPassLoadOp::Load,
                                                               .storeOp = rhi::RenderPassStoreOp::Store,
                                                               .clearColor = core::Color(0.5f, 0.5f, 0.5f, 1.0f)};
            this->colors.emplace_back(std::move(renderPassColorInfo));

            this->initializeRenderPass();
        }

        auto execute(RenderContext const& context) -> void override
        {
            context.graphics->draw(3, 1);
        }

      private:
        core::ref_ptr<Shader> shader;
    };
} // namespace ionengine::passes