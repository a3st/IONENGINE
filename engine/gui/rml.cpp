// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "rml.hpp"
#include "precompiled.h"

namespace ionengine::internal
{
    RmlRender::RmlRender(core::ref_ptr<rhi::RHI> RHI) : RHI(RHI)
    {
    }

    auto RmlRender::RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices,
                                   Rml::TextureHandle textureHandle, const Rml::Vector2f& translation) -> void
    {
        
    }
} // namespace ionengine::internal