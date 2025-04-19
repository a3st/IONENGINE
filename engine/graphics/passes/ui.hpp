// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../render_pass.hpp"
#include "../texture_allocator.hpp"

namespace ionengine::passes
{
    class UIPass : public RenderPass
    {
      public:
        UIPass(TextureAllocator& textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture);

        auto execute(RenderContext const& context, RenderableData const& renderableData) -> void override;

      private:
        core::ref_ptr<Shader> shader;
    };
} // namespace ionengine::passes