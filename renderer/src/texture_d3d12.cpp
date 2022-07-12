// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/impl/device_d3d12.hpp>
#include <renderer/impl/texture_d3d12.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

D3D12_RESOURCE_DIMENSION constexpr ionengine::renderer::to_texture_dimension(
    TextureDimension const dimension) {
    switch (dimension) {
        case TextureDimension::_1D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        case TextureDimension::Cube:
        case TextureDimension::_2D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case TextureDimension::_3D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        default:
            throw std::invalid_argument("Invalid argument");
    }
}

DXGI_FORMAT constexpr ionengine::renderer::to_texture_format(
    TextureFormat const format) {
    switch (format) {
        case TextureFormat::RGBA8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::BGRA8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case TextureFormat::BGR8_UNORM:
            return DXGI_FORMAT_B8G8R8X8_UNORM;
        case TextureFormat::R8_UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case TextureFormat::BC1_UNORM:
            return DXGI_FORMAT_BC1_UNORM;
        case TextureFormat::BC3_UNORM:
            return DXGI_FORMAT_BC3_UNORM;
        case TextureFormat::BC4_UNORM:
            return DXGI_FORMAT_BC4_UNORM;
        case TextureFormat::BC5_UNORM:
            return DXGI_FORMAT_BC5_UNORM;
        case TextureFormat::D32_FLOAT:
            return DXGI_FORMAT_R32_TYPELESS;
        case TextureFormat::RGBA16_FLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        default:
            throw std::invalid_argument("Invalid argument");
    }
}

core::Expected<std::unique_ptr<Texture>, std::string> Texture_D3D12::create(
    Device_D3D12& device, TextureDimension const dimension,
    uint32_t const width, uint32_t const height, uint16_t const mip_levels,
    uint16_t const array_layers, TextureFormat const format,
    TextureUsageFlags const flags) noexcept {
    auto texture = std::make_unique<Texture_D3D12>();

    D3D12_RESOURCE_DESC resource_desc = {
        .Dimension = to_texture_dimension(dimension),
        .Width = static_cast<uint32_t>(width),
        .Height = height,
        .DepthOrArraySize = static_cast<uint32_t>(array_layers),
        .MipLevels = static_cast<uint32_t>(mip_levels),
        .Format = to_texture_format(format),
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN};

    resource_desc.SampleDesc.Count = 1;

    D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_COMMON;

    if (flags & TextureUsageFlags::RenderTarget) {
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        initial_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    if (flags & TextureUsageFlags::DepthStencil) {
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        initial_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }
    if (flags & TextureUsageFlags::UnorderedAccess) {
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    D3D12MA::ALLOCATION_DESC allocation_desc = {.HeapType =
                                                    D3D12_HEAP_TYPE_DEFAULT};

    auto result = device._memory_allocator->CreateResource(
        &allocation_desc, &resource_desc, initial_state, nullptr,
        texture->_memory_alloc.GetAddressOf(), __uuidof(ID3D12Resource),
        reinterpret_cast<void**>(texture->_resource.GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Texture>, std::string>(
            to_string(result));
    }

    

    return core::make_expected<std::unique_ptr<Texture>, std::string>(
        std::move(texture));
}

core::Expected<std::unique_ptr<Texture>, std::string> Texture::create(
    Device& device, TextureDimension const dimension, uint32_t const width,
    uint32_t const height, uint16_t const mip_levels,
    uint16_t const array_layers, TextureFormat const format,
    TextureUsageFlags const flags) noexcept {
    return Texture_D3D12::create(static_cast<Device_D3D12&>(device), dimension,
                                 width, height, mip_levels, array_layers,
                                 format, flags);
}