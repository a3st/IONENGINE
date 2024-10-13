// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "buffer_pool.hpp"
#include "precompiled.h"

namespace ionengine
{
    ConstantBufferPool::ConstantBufferPool(rhi::Device& device) : device(&device)
    {
    }

    auto ConstantBufferPool::reset() -> void
    {
    }

    auto ConstantBufferPool::allocateMemory(size_t const size) -> core::weak_ptr<rhi::Buffer>
    {
        rhi::BufferCreateInfo bufferCreateInfo{
            .size = size,
            .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest)};
        auto buffer = device->createBuffer(bufferCreateInfo);

        buffers.emplace_back(buffer);
        return buffer;
    }
} // namespace ionengine