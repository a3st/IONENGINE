// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "surface.hpp"
#include "precompiled.h"

namespace ionengine
{
    Surface::Surface(rhi::RHI& RHI, core::ref_ptr<rhi::Buffer> vertexBuffer, size_t const indexSize,
                     uint32_t const indexCount)
        : vertexBuffer(vertexBuffer), indexCount(indexCount)
    {
        rhi::BufferCreateInfo const bufferCreateInfo{
            .size = indexSize, .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
        indexBuffer = RHI.createBuffer(bufferCreateInfo);
    }

    Surface::Surface(rhi::RHI& RHI, size_t const vertexSize, size_t const indexSize, uint32_t const indexCount)
        : indexCount(indexCount)
    {
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = vertexSize,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
            vertexBuffer = RHI.createBuffer(bufferCreateInfo);
        }

        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = indexSize,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
            indexBuffer = RHI.createBuffer(bufferCreateInfo);
        }
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