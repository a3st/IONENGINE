// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/texture.hpp"
#include "descriptor_allocator.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

auto dxgi_to_texture_format(DXGI_FORMAT const format) -> TextureFormat;

auto texture_format_to_dxgi(TextureFormat const format) -> DXGI_FORMAT;

class DX12Texture : public Texture {
public:

    DX12Texture(ID3D12Device1* device, DescriptorAllocator& allocator, TextureUsageFlags const flags);

    DX12Texture(ID3D12Device1* device, winrt::com_ptr<ID3D12Resource> resource, DescriptorAllocator& allocator, TextureUsageFlags const flags);

    ~DX12Texture();

    auto get_width() const -> uint32_t override {
        
        return width;
    }

    auto get_height() const -> uint32_t override {

        return height;
    }

    auto get_depth() const -> uint32_t override {

        return depth;
    }

    auto get_mip_levels() const -> uint32_t override {

        return mip_levels;
    }

    auto get_format() const -> TextureFormat override {

        return format;
    }

    auto get_descriptor() -> DescriptorAllocation {

        return descriptor_allocation;
    }

    auto get_resource() -> ID3D12Resource* {

        return resource.get();
    }

    auto get_resource_state() const -> D3D12_RESOURCE_STATES {

        return resource_state;
    }

    auto set_resource_state(D3D12_RESOURCE_STATES const state) -> void {

        resource_state = state;
    }

private:

    DescriptorAllocator* descriptor_allocator;
    winrt::com_ptr<ID3D12Resource> resource;
    DescriptorAllocation descriptor_allocation;
    D3D12_RESOURCE_STATES resource_state;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    TextureFormat format;
    TextureUsageFlags flags;
};

}

}

}