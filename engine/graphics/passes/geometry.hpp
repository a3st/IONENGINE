// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../render_pass.hpp"
#include "../texture_allocator.hpp"

namespace ionengine::passes
{
    class GeometryPass : public RenderPass
    {
      public:
        GeometryPass(TextureAllocator& textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture);

        auto execute(RenderContext const& context, RenderableData const& renderableData) -> void override;

      private:
        core::ref_ptr<rhi::Texture> cameraTexture;
    };
} // namespace ionengine::passes