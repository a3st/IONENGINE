// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "texture.hpp"
#include "precompiled.h"

namespace ionengine
{
    TextureAllocator::TextureAllocator(core::ref_ptr<rhi::RHI> RHI) : RHI(RHI)
    {
    }

    auto TextureAllocator::allocate(rhi::TextureCreateInfo const& createInfo) -> core::weak_ptr<rhi::Texture>
    {
        bool allocatedMemory = true;
        core::weak_ptr<rhi::Texture> outTexture;

        Entry const entry{.width = createInfo.width,
                          .height = createInfo.height,
                          .depth = createInfo.depth,
                          .mipLevels = createInfo.mipLevels,
                          .format = createInfo.format,
                          .dimension = createInfo.dimension,
                          .flags = createInfo.flags};
        auto bucketResult = buckets.find(entry);
        if (bucketResult != buckets.end())
        {
            if (bucketResult->second.cur < bucketResult->second.elements.size())
            {
                outTexture = bucketResult->second.elements[bucketResult->second.cur++];
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
            auto texture = RHI->createTexture(createInfo);
            textures.emplace_back(texture);

            if (buckets.find(entry) != buckets.end())
            {
                auto& bucket = buckets[entry];

                bucket.elements.emplace_back(texture);
                ++bucket.cur;
            }
            else
            {
                Bucket bucket{.cur = 1};
                bucket.elements.emplace_back(texture);

                buckets[entry] = std::move(bucket);
            }

            outTexture = texture;
        }
        return outTexture;
    }

    auto TextureAllocator::reset() -> void
    {
        for (auto& [_, bucketData] : buckets)
        {
            bucketData.cur = 0;
        }
    }
} // namespace ionengine