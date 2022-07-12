// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/texture.hpp>

#define NOMINMAX
#include <d3d12.h>
#include <d3d12ma/D3D12MemAlloc.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace Microsoft;

namespace ionengine::renderer {

class Texture_D3D12 final : public Texture {
 public:
    static core::Expected<std::unique_ptr<Texture>, std::string> create(
        Device_D3D12& device, TextureDimension const dimension, uint32_t const width,
        uint32_t const height, uint16_t const mip_levels,
        uint16_t const array_layers, TextureFormat const format,
        TextureUsageFlags const flags) noexcept;

 private:
    WRL::ComPtr<ID3D12Resource> _resource;
    WRL::ComPtr<D3D12MA::Allocation> _memory_alloc;
};

D3D12_RESOURCE_DIMENSION constexpr to_texture_dimension(TextureDimension const dimension);
DXGI_FORMAT constexpr to_texture_format(TextureFormat const format);

}  // namespace ionengine::renderer