// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include <webgpu/webgpu.hpp>

namespace ionengine {

namespace renderer {

class Backend;

enum class TextureDimension {
    _2D,
    Cube,
    _3D
};

class Texture : public core::ref_counted_object {
public:

    Texture(
        Backend& backend,
        uint32_t const width,
        uint32_t const height,
        uint32_t const depth,
        uint32_t const mip_levels,
        wgpu::TextureFormat const format,
        TextureDimension const dimension,
        uint32_t const sample_count,
        wgpu::TextureUsageFlags const usage
    );

    auto get_view() const -> wgpu::TextureView {

        return view;
    }

    auto get_texture() const -> wgpu::Texture {
        
        return texture;
    }

    auto is_uploaded(uint32_t const mip_level) -> bool {

        return uploads[mip_level];
    }

    // auto upload(, uint32_t const mip_level, std::span<uint8_t const> const data) -> void;

private:

    Backend* backend;
    wgpu::Texture texture{nullptr};
    wgpu::TextureView view{nullptr};
    std::array<uint32_t, 16> uploads;
};

class Texture2D : public Texture {
public:

    Texture2D(
        Backend& backend,
        uint32_t const width,
        uint32_t const height,
        uint32_t const mip_levels,
        wgpu::TextureFormat const format,
        uint32_t const sample_count,
        wgpu::TextureUsageFlags const usage
    ) : Texture(backend, width, height, 1, mip_levels, format, TextureDimension::_2D, sample_count, usage) { }
};

class TextureCube : public Texture {
public:

    TextureCube(
        Backend& backend,
        uint32_t const width,
        uint32_t const height,
        uint32_t const mip_levels,
        wgpu::TextureFormat const format,
        uint32_t const sample_count,
        wgpu::TextureUsageFlags const usage
    ) : Texture(backend, width, height, 6, mip_levels, format, TextureDimension::Cube, sample_count, usage) { }
};

struct TextureData {
    wgpu::TextureFormat format;
    TextureDimension dimension;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    std::array<std::span<uint8_t const>, 16> data;
};

}

}