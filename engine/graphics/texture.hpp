// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include <xxhash.h>

namespace ionengine
{
    class TextureAllocator : public core::ref_counted_object
    {
      public:
        struct Entry
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mipLevels;
            rhi::TextureFormat format;
            rhi::TextureDimension dimension;
            rhi::TextureUsageFlags flags;

            auto operator==(Entry const& other) const -> bool
            {
                return std::make_tuple(width, height, depth, mipLevels, format, dimension, flags) ==
                       std::make_tuple(other.width, other.height, other.depth, other.mipLevels, other.format,
                                       other.dimension, other.flags);
            }
        };

        struct EntryHasher
        {
            auto operator()(const Entry& other) const -> std::size_t
            {
                return ::XXH64(&other.width, sizeof(uint32_t), 0) ^ ::XXH64(&other.height, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.depth, sizeof(uint32_t), 0) ^ ::XXH64(&other.mipLevels, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.format, sizeof(uint32_t), 0) ^ ::XXH64(&other.dimension, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.flags, sizeof(uint32_t), 0);
            }
        };

        TextureAllocator(core::ref_ptr<rhi::RHI> RHI);

        auto allocate(rhi::TextureCreateInfo const& createInfo) -> core::weak_ptr<rhi::Texture>;

        auto reset() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;

        std::vector<core::ref_ptr<rhi::Texture>> textures;

        struct Bucket
        {
            std::vector<core::ref_ptr<rhi::Texture>> elements;
            uint32_t cur;
        };

        std::unordered_map<Entry, Bucket, EntryHasher> buckets;
    };
} // namespace ionengine