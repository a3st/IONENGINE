// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "texture_pool.hpp"
#include "precompiled.h"

namespace ionengine
{
    TexturePool::TexturePool(core::ref_ptr<rhi::RHI> rhi) : _rhi(rhi)
    {
    }

    auto TexturePool::allocate(rhi::TextureCreateInfo const& createInfo) -> core::weak_ptr<rhi::Texture>
    {
        bool allocTexture = true;
        core::weak_ptr<rhi::Texture> outTexture;

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
            if (bucketResult->second.current < bucketResult->second.textures.size())
            {
                outTexture = bucketResult->second.textures[bucketResult->second.current++];
            }
            else
            {
                allocTexture = false;
            }
        }
        else
        {
            allocTexture = false;
        }

        if (!allocTexture)
        {
            auto texture = _rhi->createTexture(createInfo);

            if (_buckets.find(entry) != _buckets.end())
            {
                auto& bucket = _buckets[entry];

                bucket.textures.emplace_back(texture);
                ++bucket.current;
            }
            else
            {
                std::vector<core::ref_ptr<rhi::Texture>> textures;
                textures.emplace_back(texture);

                Bucket bucket{.textures = std::move(textures), .current = 1};
                _buckets[entry] = std::move(bucket);
            }

            outTexture = texture;
        }
        return outTexture;
    }

    auto TexturePool::reset() -> void
    {
        for (auto& [_, bucket] : _buckets)
        {
            bucket.current = 0;
        }
    }

    auto TexturePool::clear() -> void
    {
        _buckets.clear();
    }
} // namespace ionengine