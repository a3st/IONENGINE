// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/texture.hpp>
#include <renderer/impl/vk.hpp>

namespace ionengine::renderer {

///
/// @private
///
class Texture_VK final : public Texture {
 public:
    static core::Expected<std::unique_ptr<Texture>, std::string> create(
        Device_VK& device, TextureDimension const dimension, uint32_t const width,
        uint32_t const height, uint16_t const mip_levels,
        uint16_t const array_layers, TextureFormat const format,
        TextureUsageFlags const flags) noexcept;

 private:
    
};
    
}