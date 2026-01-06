// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "texture_pool.hpp"
#include "precompiled.h"

namespace ionengine
{
    TexturePool::TexturePool(core::ref_ptr<rhi::RHI> rhi) : _rhi(rhi)
    {
    }

    auto TexturePool::allocate(rhi::TextureCreateInfo const& createInfo) -> std::optional<Allocation>
    {
        std::lock_guard lock(_mutex);

        std::optional<Allocation> textureAllocation;

        Entry const entry{.width = createInfo.width,
                          .height = createInfo.height,
                          .depth = createInfo.depth,
                          .mipLevels = createInfo.mipLevels,
                          .format = createInfo.format,
                          .dimension = createInfo.dimension,
                          .flags = createInfo.flags};
        auto bucketResult = _buckets.find(entry);
        if (bucketResult != _buckets.end())
        {
            Bucket& bucket = bucketResult->second;

            textureAllocation = getTextureInBucket(bucket, entry);
            if (!textureAllocation.has_value())
            {
                textureAllocation = createTextureInBucket(bucket, entry);
            }
        }
        else
        {
            Bucket newBucket{};
            auto insertResult = _buckets.emplace(entry, std::move(newBucket));
            if (insertResult.second)
            {
                textureAllocation = createTextureInBucket(insertResult.first->second, entry);
            }
        }
        return textureAllocation;
    }

    auto TexturePool::createTextureInBucket(Bucket& bucket, Entry const& entry) -> Allocation
    {
        rhi::TextureCreateInfo const textureCreateInfo{.width = entry.width,
                                                       .height = entry.height,
                                                       .depth = entry.depth,
                                                       .mipLevels = entry.mipLevels,
                                                       .format = entry.format,
                                                       .dimension = entry.dimension,
                                                       .flags = entry.flags};
        core::ref_ptr<rhi::Texture> newTexture = _rhi->createTexture(textureCreateInfo);

        Bucket::Entry bucketEntry{.texture = newTexture, .used = true, .initialState = rhi::ResourceState::Common};
        bucket.entries.emplace_back(std::move(bucketEntry));
        return Allocation(entry, bucket.current++, newTexture.get(), rhi::ResourceState::Common);
    }

    auto TexturePool::getTextureInBucket(Bucket& bucket, Entry const& entry) -> std::optional<Allocation>
    {
        for (uint32_t const i : std::views::iota(bucket.current, bucket.entries.size()))
        {
            Bucket::Entry& bucketEntry = bucket.entries[i];

            if (!bucketEntry.used)
            {
                bucketEntry.used = true;
                bucket.current = i + 1;

                if (bucketEntry.initialState != rhi::ResourceState::Common)
                {
                    bucket.compactCandidates.erase(i);
                }

                return std::make_optional<Allocation>(entry, i, bucketEntry.texture.get(), bucketEntry.initialState);
            }
        }
        return std::nullopt;
    }

    auto TexturePool::deallocate(Allocation const& allocation, rhi::ResourceState const initialState) -> void
    {
        std::lock_guard lock(_mutex);

        auto bucketResult = _buckets.find(allocation._entry);
        if (bucketResult != _buckets.end())
        {
            Bucket& bucket = bucketResult->second;

            if (allocation._current < bucket.entries.size())
            {
                Bucket::Entry& bucketEntry = bucket.entries[allocation._current];

                bucketEntry.used = false;
                bucketEntry.initialState = initialState;

                if (allocation._current < bucket.current)
                {
                    bucket.current = allocation._current;
                }

                if (initialState != rhi::ResourceState::Common)
                {
                    _dirtyEntries.emplace(allocation._entry);
                    bucket.compactCandidates.emplace(allocation._current);
                }
            }
        }
    }

    auto TexturePool::compact() -> void
    {
        for (auto const& entry : _dirtyEntries)
        {
            auto bucketResult = _buckets.find(entry);
            if (bucketResult != _buckets.end())
            {
                Bucket& bucket = bucketResult->second;

                for (auto const index : bucket.compactCandidates)
                {
                    if (index < bucket.entries.size())
                    {
                        Bucket::Entry& bucketEntry = bucket.entries[index];

                        rhi::ResourceState const afterState = rhi::ResourceState::Common;
                        _rhi->getGraphicsContext()->barrier(bucketEntry.texture.get(), bucketEntry.initialState,
                                                            afterState);
                        bucketEntry.initialState = afterState;
                    }
                }
            }
        }
    }
} // namespace ionengine