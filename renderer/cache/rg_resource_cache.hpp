// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/device.hpp"
#include <xxhash/xxhash64.h>

namespace ionengine {

namespace renderer {

struct RGResource {
    core::ref_ptr<rhi::Texture> texture{nullptr};
};

class RGResourceCache {
public:

    struct Entry {
        uint32_t frame_index;
        rhi::TextureFormat format;
        uint32_t sample_count;
        uint32_t width;
        uint32_t height;

        auto operator==(Entry const& other) const -> bool {
            return std::tie(frame_index, format, sample_count, width, height) == 
                std::tie(other.frame_index, other.format, other.sample_count, other.width, other.height);
        }
    };

    struct EntryHasher {
        auto operator()(const Entry& entry) const -> std::size_t {
            return 
                XXHash64::hash(&entry.frame_index, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.format, sizeof(rhi::TextureFormat), 0) ^
                XXHash64::hash(&entry.sample_count, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.width, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.height, sizeof(uint32_t), 0)
            ;
        }
    };

    RGResourceCache(rhi::Device& device);

    auto get(
        uint32_t const frame_index,
        rhi::TextureFormat const format, 
        uint32_t const sample_count, 
        uint32_t const width, 
        uint32_t const height,
        uint64_t const hash
    ) -> RGResource;

    auto update() -> void;

private:

    inline static uint32_t const DEFAULT_LIFETIME_VALUE = 1; 

    rhi::Device* device;

    struct Region {
        std::vector<RGResource> resources;
        std::vector<uint32_t> free;
        std::vector<uint32_t> lifetimes;
        uint32_t offset;
    };
    std::vector<Region> regions;
    std::unordered_map<Entry, Region, EntryHasher> entries;

    struct ResourceAllocation {
        Region* region;
        uint32_t offset;
    };
    std::unordered_map<uint64_t, ResourceAllocation> allocations;
};

}

}