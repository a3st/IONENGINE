// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/device.hpp"
#include <xxhash/xxhash64.h>

namespace ionengine {

namespace renderer {

class RGResourceCache : public core::ref_counted_object {
public:

    struct Entry {
        uint32_t frame_index;
        rhi::TextureFormat format;
        uint32_t sample_count;
        uint32_t width;
        uint32_t height;
        rhi::TextureUsageFlags flags;

        auto operator==(Entry const& other) const -> bool {
            return std::tie(frame_index, format, sample_count, width, height, flags) == 
                std::tie(other.frame_index, other.format, other.sample_count, other.width, other.height, flags);
        }
    };

    struct EntryHasher {
        auto operator()(const Entry& entry) const -> std::size_t {
            return 
                XXHash64::hash(&entry.frame_index, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.format, sizeof(rhi::TextureFormat), 0) ^
                XXHash64::hash(&entry.sample_count, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.width, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.height, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.flags, sizeof(uint32_t), 0)
            ;
        }
    };

    RGResourceCache(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator);

    auto get(
        uint32_t const frame_index,
        rhi::TextureFormat const format, 
        uint32_t const sample_count, 
        uint32_t const width, 
        uint32_t const height,
        rhi::TextureUsageFlags const flags,
        uint64_t const hash
    ) -> core::ref_ptr<rhi::Texture>;

    auto update() -> void;

private:

    inline static uint32_t const DEFAULT_LIFETIME_VALUE = 1; 

    rhi::Device* device;
    core::ref_ptr<rhi::MemoryAllocator> allocator;

    using TimedResource = std::pair<core::ref_ptr<rhi::Texture>, uint64_t>;

    struct Chunk {
        std::vector<TimedResource> resources;
        std::vector<uint32_t> free;
        uint32_t offset;
    };
    std::unordered_map<Entry, std::unique_ptr<Chunk>, EntryHasher> entries;

    struct ResourceAllocation {
        Chunk* chunk;
        uint32_t offset;
    };
    std::unordered_map<uint64_t, ResourceAllocation> allocations;
};

}

}