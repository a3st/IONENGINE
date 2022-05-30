// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>
#include <lib/expected.h>
#include <lib/hash/buffer.h>

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

    lib::hash::Buffer<uint8_t> data;
    uint32_t width;
    uint32_t height;
    TextureFormat format;
    TextureFilter filter;
    uint32_t mip_count;
    uint64_t hash;

    size_t cache_entry{std::numeric_limits<size_t>::max()};

    static lib::Expected<Texture, lib::Result<TextureError>> load_from_file(std::filesystem::path const& file_path);
};

}