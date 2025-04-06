// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "ui.hpp"
#include "precompiled.h"

namespace ionengine::passes
{
    UIPass::UIPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture,
                   core::ref_ptr<Shader> shader, core::ref_ptr<rhi::Texture> swapchainTexture)
        : RenderPass("UI Pass")
    {
    }

    auto UIPass::execute(RenderContext const& context, RenderableData const& renderableData) -> void
    {
    }
} // namespace ionengine::passes