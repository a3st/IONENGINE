// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "buffer_pool.hpp"
#include "precompiled.h"

namespace ionengine
{
    BufferPool::BufferPool(core::ref_ptr<rhi::RHI> rhi, rhi::BufferUsage const bufferUsage)
        : _rhi(rhi), _bufferUsage(bufferUsage)
    {
    }

    auto BufferPool::allocate(size_t const bufferSize) -> core::weak_ptr<rhi::Buffer>
    {
        bool allocBuffer = true;
        core::weak_ptr<rhi::Buffer> outBuffer;

        auto bucketResult = _buckets.upper_bound(bufferSize - 1);
        if (bucketResult != _buckets.end())
        {
            if (bucketResult->second.current < bucketResult->second.buffers.size())
            {
                outBuffer = bucketResult->second.buffers[bucketResult->second.current++];
            }
            else
            {
                allocBuffer = false;
            }
        }
        else
        {
            allocBuffer = false;
        }

        if (!allocBuffer)
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = bufferSize, .flags = (rhi::BufferUsageFlags)_bufferUsage | rhi::BufferUsage::CopyDest};
            auto buffer = _rhi->createBuffer(bufferCreateInfo);

            if (_buckets.upper_bound(bufferSize) != _buckets.end())
            {
                auto& bucket = _buckets[bufferSize];

                bucket.buffers.emplace_back(buffer);
                ++bucket.current;
            }
            else
            {
                std::vector<core::ref_ptr<rhi::Buffer>> buffers;
                buffers.emplace_back(buffer);

                Bucket bucket{.buffers = std::move(buffers), .current = 1};
                _buckets[bufferSize] = std::move(bucket);
            }

            outBuffer = buffer;
        }
        return outBuffer;
    }

    auto BufferPool::reset() -> void
    {
        for (auto& [_, bucket] : _buckets)
        {
            bucket.current = 0;
        }
    }
} // namespace ionengine