// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/graphics/render_pass.hpp"

namespace ionengine::passes
{
    class SwapchainPass : public RenderPass
    {
      public:
        SwapchainPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture,
                      core::ref_ptr<Shader> shader, core::ref_ptr<rhi::Texture> swapchainTexture)
            : RenderPass("Swapchain Pass")
        {
            rhi::RenderPassColorInfo const renderPassColorInfo{.texture = swapchainTexture,
                                                               .loadOp = rhi::RenderPassLoadOp::Clear,
                                                               .storeOp = rhi::RenderPassStoreOp::Store,
                                                               .clearColor = core::Color(0.0f, 0.0f, 0.0f, 1.0f)};
            this->colors.emplace_back(std::move(renderPassColorInfo));
            this->shader = shader;

            RenderPassInputInfo const renderPassInputInfo{.bindingName = "inputTexture1", .texture = cameraTexture};
            this->inputs.emplace_back(std::move(renderPassInputInfo));

            this->initializeRenderPass();
        }

        auto execute(rhi::GraphicsContext* context) -> void override
        {
            context->draw(3, 1);
        }
    };
} // namespace ionengine::passes