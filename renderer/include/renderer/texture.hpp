// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.hpp>

namespace ionengine::renderer {

class Device;

enum class TextureFormat {
    BGRA8_UNORM,
    BGR8_UNORM,
    RGBA8_UNORM,
    R8_UNORM,
    BC1_UNORM,
    BC3_UNORM,
    BC4_UNORM,
    BC5_UNORM,
    D32_FLOAT,
    RGBA16_FLOAT
};

enum class TextureDimension { _1D, _2D, _3D, Cube };

enum class TextureUsageFlags : uint32_t {
    ShaderResource = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    UnorderedAccess = 1 << 3
};

DECLARE_ENUM_CLASS_BIT_FLAG(TextureUsageFlags)

class Texture {
 public:
    ///
    /// Creates texture
    /// @param device Device
    /// @param dimension Dimension texture
    /// @param width Width texture
    /// @param height Height texture
    /// @param mip_levels Mip levels texture
    /// @param array_layers Array levels texture
    /// @param format Format texture
    /// @param flags Usage flags texture
    /// @return Returns unqiue pointer to texture
    ///
    static core::Expected<std::unique_ptr<Texture>, std::string> create(
        Device& device, TextureDimension const dimension, uint32_t const width,
        uint32_t const height, uint16_t const mip_levels,
        uint16_t const array_layers, TextureFormat const format,
        TextureUsageFlags const flags) noexcept;

    static core::Expected<std::unique_ptr<Texture>, std::string> from_swapchain(
        Device& device, uint32_t const buffer_index) noexcept;
};

}  // namespace ionengine::renderer