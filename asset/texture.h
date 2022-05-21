// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>
#include <lib/expected.h>
#include <lib/hash/buffer.h>
#include <dds/DDS.h>

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

class Texture {
public:

    static lib::Expected<Texture, lib::Result<TextureError>> load_from_file(std::filesystem::path const& file_path);

    uint64_t hash() const;

    void cache_entry(size_t const value);

    size_t cache_entry() const;

    TextureFormat format() const;

    uint32_t width() const;

    uint32_t height() const;

    uint32_t mip_count() const;

    std::span<uint8_t const> data() const;

private:

    Texture(DirectX::DDS_HEADER const& header, std::span<uint8_t const> const mip_data);

    lib::hash::Buffer<uint8_t> _data;
    uint32_t _width;
    uint32_t _height;
    TextureFormat _format;
    TextureFilter _filter;
    uint32_t _mip_count;

    size_t _cache_entry{std::numeric_limits<size_t>::max()};
};

}