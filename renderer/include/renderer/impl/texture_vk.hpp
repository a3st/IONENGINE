// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/texture.hpp>

#if defined(_WIN64)
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.hpp>

namespace ionengine::renderer {

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