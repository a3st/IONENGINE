// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../render_pass.hpp"
#include "../../texture_allocator.hpp"

namespace ionengine::passes
{
    class SwapchainPass : public RenderPass
    {
      public:
        SwapchainPass(TextureAllocator& textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture,
                      core::ref_ptr<Shader> shader, core::ref_ptr<rhi::Texture> swapchainTexture);

        auto execute(RenderContext const& context, RenderableData const& renderableData) -> void override;

      private:
        core::ref_ptr<Shader> shader;
    };
} // namespace ionengine::passes