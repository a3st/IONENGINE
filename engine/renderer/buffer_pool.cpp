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
        for (auto [size, bucketData] : buckets)
        {
            bucketData.cur = bucketData.elements.begin();
        }
    }

    auto ConstantBufferPool::allocateMemory(size_t const size) -> core::weak_ptr<rhi::Buffer>
    {
        bool allocatedMemory = true;
        core::weak_ptr<rhi::Buffer> outBuffer;

        auto result = buckets.upper_bound(size);
        if (result != buckets.end())
        {
            if (result->second.cur != result->second.elements.end())
            {
                auto buffer = *(*result->second.cur);
                ++result->second.cur;
                outBuffer = buffer;
            }
            else
            {
                allocatedMemory = false;
            }
        }
        else
        {
            allocatedMemory = false;
        }

        if (!allocatedMemory)
        {
            rhi::BufferCreateInfo bufferCreateInfo{
                .size = size,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest)};
            auto buffer = device->createBuffer(bufferCreateInfo);

            buffers.emplace_back(buffer);

            if (buckets.find(size) != buckets.end())
            {
                buckets[size].elements.emplace_back(buffers.rbegin().base());
            }
            else
            {
                Entry entry{};
                entry.elements.emplace_back(buffers.rbegin().base());
                entry.cur = entry.elements.begin();

                buckets[size] = std::move(entry);
            }

            outBuffer = buffer;
        }
        return outBuffer;
    }
} // namespace ionengine