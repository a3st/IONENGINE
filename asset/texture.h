// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>
#include <lib/expected.h>
#include <lib/hash/buffer.h>
#include <lib/math/color.h>

namespace ionengine::asset {

enum class TextureError {
    IO,
    ParseError
};

enum class TextureFormat {
    BC1,
    BC2,
    BC3,
    BC4,
    BC5,
    RGBA8_UNORM
};

enum class TextureFilter {
    MinMagMipLinear
};

enum class TextureAddress {
    Wrap,
    Clamp,
    Mirror
};

struct Texture {
    lib::hash::Buffer<uint8_t> data;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    TextureFormat format;
    TextureFilter filter;
    TextureAddress s_address_mode;
    TextureAddress t_address_mode;
    uint32_t mip_count;
    uint64_t hash;

    bool is_cube_map{false};

    bool is_render_target{false};
    size_t cache_entry{std::numeric_limits<size_t>::max()};

    static lib::Expected<Texture, lib::Result<TextureError>> create(uint32_t const width, uint32_t const height, bool const is_render_target, lib::math::Color const& initial_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f));
    
    static lib::Expected<Texture, lib::Result<TextureError>> load_from_file(std::filesystem::path const& file_path);
};

}