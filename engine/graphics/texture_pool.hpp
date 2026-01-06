// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include <xxhash.h>

namespace ionengine
{
    class TexturePool : public core::ref_counted_object
    {
        struct Bucket
        {
            struct Entry
            {
                core::ref_ptr<rhi::Texture> texture;
                bool used;
                rhi::ResourceState initialState;
            };

            std::vector<Entry> entries;
            uint32_t current;
            std::unordered_set<uint32_t> compactCandidates;
        };

        struct Entry
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mipLevels;
            rhi::Format format;
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
            auto operator()(Entry const& other) const -> std::size_t
            {
                XXH64_state_t* hasher = ::XXH64_createState();
                ::XXH64_reset(hasher, 0);

                ::XXH64_update(hasher, &other.width, sizeof(uint32_t));
                ::XXH64_update(hasher, &other.height, sizeof(uint32_t));
                ::XXH64_update(hasher, &other.depth, sizeof(uint32_t));
                ::XXH64_update(hasher, &other.mipLevels, sizeof(uint32_t));
                ::XXH64_update(hasher, &other.format, sizeof(uint32_t));
                ::XXH64_update(hasher, &other.dimension, sizeof(uint32_t));
                ::XXH64_update(hasher, &other.flags, sizeof(uint32_t));

                uint64_t const hash = ::XXH64_digest(hasher);
                ::XXH64_freeState(hasher);
                return hash;
            }
        };

      public:
        class Allocation
        {
            friend class TexturePool;

          public:
            Allocation(Entry const& entry, uint32_t const current, rhi::Texture* const texture,
                       rhi::ResourceState const initialState)
                : _entry(entry), _current(current), _texture(texture), _initialState(initialState)
            {
            }

            auto getTexture() const -> rhi::Texture*
            {
                return _texture;
            }

            auto getInitialState() const -> rhi::ResourceState
            {
                return _initialState;
            }

          private:
            Entry _entry;
            uint32_t _current;
            rhi::Texture* _texture;
            rhi::ResourceState _initialState;
        };

        explicit TexturePool(core::ref_ptr<rhi::RHI> rhi);

        auto allocate(rhi::TextureCreateInfo const& createInfo) -> std::optional<Allocation>;

        auto deallocate(Allocation const& allocation, rhi::ResourceState const initialState) -> void;

        auto compact() -> void;

      private:
        core::ref_ptr<rhi::RHI> _rhi;
        std::mutex _mutex;
        std::unordered_map<Entry, Bucket, EntryHasher> _buckets;
        std::unordered_set<Entry, EntryHasher> _dirtyEntries;

        auto createTextureInBucket(Bucket& bucket, Entry const& entry) -> Allocation;

        auto getTextureInBucket(Bucket& bucket, Entry const& entry) -> std::optional<Allocation>;
    };
} // namespace ionengine