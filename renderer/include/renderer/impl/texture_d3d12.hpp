// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/impl/d3d12.hpp>
#include <renderer/texture.hpp>

namespace ionengine::renderer {

///
/// @private
///
class Texture_D3D12 final : public Texture {
 public:
    static core::Expected<std::unique_ptr<Texture>, std::string> create(
        Device_D3D12& device, TextureDimension const dimension,
        uint32_t const width, uint32_t const height, uint16_t const mip_levels,
        uint16_t const array_layers, TextureFormat const format,
        TextureUsageFlags const flags) noexcept;

    static std::unique_ptr<Texture> from_swapchain(
        Device_D3D12& device, uint32_t const buffer_index) noexcept;

 private:
    WRL::ComPtr<ID3D12Resource> _resource;
    WRL::ComPtr<D3D12MA::Allocation> _memory_alloc;
    bool swapchain_used;
};

///
/// @private
///
D3D12_RESOURCE_DIMENSION constexpr to_texture_dimension(
    TextureDimension const dimension);

///
/// @private
///
DXGI_FORMAT constexpr to_texture_format(TextureFormat const format);

}  // namespace ionengine::renderer