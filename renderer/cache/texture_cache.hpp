// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/device.hpp"

namespace ionengine {

namespace renderer {

struct TextureData {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    rhi::TextureFormat format;
    rhi::TextureDimension dimension;
    std::array<std::span<uint8_t const>, 16> mip_data;
    uint64_t hash;
};

class TextureCache : public core::ref_counted_object {
public:

    TextureCache(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator);

    auto get(TextureData const& data, bool const immediate = false) -> std::optional<core::ref_ptr<rhi::Texture>>;

    auto update(float const dt) -> void;

private:

    inline static uint32_t ALLOCATE_SIZE_PER_FRAME = 8 * 1024 * 1024;
    size_t size{0};

    rhi::Device* device;
    core::ref_ptr<rhi::MemoryAllocator> allocator;

    struct TimedResource {
        rhi::Future<rhi::Texture> texture;
        uint64_t lifetime;
    };
    std::unordered_map<uint64_t, TimedResource> entries;
};

}

}