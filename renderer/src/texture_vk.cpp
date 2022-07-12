// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/impl/device_vk.hpp>
#include <renderer/impl/texture_vk.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

core::Expected<std::unique_ptr<Texture>, std::string> Texture_VK::create(
    Device_VK& device, TextureDimension const dimension,
    uint32_t const width, uint32_t const height, uint16_t const mip_levels,
    uint16_t const array_layers, TextureFormat const format,
    TextureUsageFlags const flags) noexcept {

    return core::make_expected<std::unique_ptr<Texture>, std::string>("VK_ERROR");
}

core::Expected<std::unique_ptr<Texture>, std::string> Texture::create(
    Device& device, TextureDimension const dimension, uint32_t const width,
    uint32_t const height, uint16_t const mip_levels,
    uint16_t const array_layers, TextureFormat const format,
    TextureUsageFlags const flags) noexcept {
    return Texture_VK::create(static_cast<Device_VK&>(device), dimension,
                                 width, height, mip_levels, array_layers,
                                 format, flags);
}