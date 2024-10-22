// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "surface.hpp"
#include "precompiled.h"

namespace ionengine
{
    Surface::Surface(core::ref_ptr<rhi::Buffer> vertexBuffer, core::ref_ptr<rhi::Buffer> indexBuffer,
                     uint32_t const indexCount)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), indexCount(indexCount)
    {
    }

    auto Surface::draw(rhi::GraphicsContext& context) -> void
    {
        context.bindVertexBuffer(vertexBuffer, 0, vertexBuffer->getSize());
        context.bindIndexBuffer(indexBuffer, 0, indexBuffer->getSize(), rhi::IndexFormat::Uint32);
        context.drawIndexed(indexCount, 1);
    }
}