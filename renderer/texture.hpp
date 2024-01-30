// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"

namespace ionengine {

namespace renderer {

class Texture : public core::ref_counted_object {
public:

    Texture(
        rhi::Device& device,
        core::ref_ptr<rhi::MemoryAllocator> allocator,
        uint32_t const width,
        uint32_t const height,
        uint32_t const depth,
        uint32_t const mip_levels,
        rhi::TextureFormat const format,
        rhi::TextureDimension const dimension,
        std::vector<std::span<uint8_t const>> const& data,
        bool const immediate = false
    );

    auto get_texture() -> core::ref_ptr<rhi::Texture> {

        return texture.get();
    }

private:

    rhi::Future<rhi::Texture> texture;
};

}

}