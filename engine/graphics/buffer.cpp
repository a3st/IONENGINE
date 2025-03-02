// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "buffer.hpp"
#include "precompiled.h"

namespace ionengine
{
    BufferAllocator::BufferAllocator(core::ref_ptr<rhi::RHI> RHI) : RHI(RHI)
    {
    }

    auto BufferAllocator::allocate(size_t const size) -> core::weak_ptr<rhi::Buffer>
    {
        bool allocatedMemory = true;
        core::weak_ptr<rhi::Buffer> outBuffer;

        auto bucketResult = buckets.upper_bound(size);
        if (bucketResult != buckets.end())
        {
            if (bucketResult->second.cur < bucketResult->second.elements.size())
            {
                outBuffer = bucketResult->second.elements[bucketResult->second.cur++];
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
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = size,
                .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest};
            auto buffer = RHI->createBuffer(bufferCreateInfo);
            buffers.emplace_back(buffer);

            if (buckets.upper_bound(size) != buckets.end())
            {
                auto& bucket = buckets[size];

                bucket.elements.emplace_back(buffer);
                ++bucket.cur;
            }
            else
            {
                Bucket bucket{.cur = 1};
                bucket.elements.emplace_back(buffer);

                buckets[size] = std::move(bucket);
            }

            outBuffer = buffer;
        }
        return outBuffer;
    }

    auto BufferAllocator::reset() -> void
    {
        for (auto& [_, bucketData] : buckets)
        {
            bucketData.cur = 0;
        }
    }
} // namespace ionengine