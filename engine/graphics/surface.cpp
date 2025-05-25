// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "surface.hpp"
#include "precompiled.h"

namespace ionengine
{
    Surface::Surface(core::ref_ptr<rhi::Buffer> const& vertexBuffer, core::ref_ptr<rhi::Buffer> const& indexBuffer,
                     uint32_t const indexCount)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), indexCount(indexCount)
    {
    }

    auto Surface::draw(rhi::GraphicsContext* context) const -> void
    {
        context->bindVertexBuffer(vertexBuffer, 0, vertexBuffer->getSize());
        context->bindIndexBuffer(indexBuffer, 0, indexBuffer->getSize(), rhi::IndexFormat::Uint32);
        context->drawIndexed(indexCount, 1);
    }

    auto Surface::getVertexBuffer() const -> core::ref_ptr<rhi::Buffer>
    {
        return vertexBuffer;
    }

    auto Surface::getIndexBuffer() const -> core::ref_ptr<rhi::Buffer>
    {
        return indexBuffer;
    }
} // namespace ionengine