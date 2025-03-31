// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/graphics/render_pass.hpp"

namespace ionengine::passes
{
    class GeometryPass : public RenderPass
    {
      public:
        GeometryPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture);

        auto execute(RenderContext const& context, RenderableData const& renderableData) -> void override;
    };
} // namespace ionengine::passes