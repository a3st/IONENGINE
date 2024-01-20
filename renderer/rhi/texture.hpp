// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

enum class TextureFormat {
    Unknown,
    BGRA8_UNORM,
    BGR8_UNORM,
    RGBA8_UNORM,
    R8_UNORM,
    BC1,
    BC3,
    BC4,
    BC5,
    D32_FLOAT,
    RGBA16_FLOAT
};

enum class TextureDimension {
    _1D,
    _2D,
    _2DArray,
    _3D,
    Cube,
    CubeArray
};

enum class TextureUsage {
    ShaderResource = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    UnorderedAccess = 1 << 3,
    CopySrc = 1 << 4,
    CopyDst = 1 << 5
};

DECLARE_ENUM_CLASS_BIT_FLAG(TextureUsage)

class Texture : public core::ref_counted_object {
public:

    virtual auto get_width() const -> uint32_t = 0;

    virtual auto get_height() const -> uint32_t = 0;

    virtual auto get_depth() const -> uint32_t = 0;

    virtual auto get_mip_levels() const -> uint32_t = 0;

    virtual auto get_format() const -> TextureFormat = 0;
};

}

}

}