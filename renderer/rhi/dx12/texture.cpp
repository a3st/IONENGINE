// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "texture.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

auto ionengine::renderer::rhi::dxgi_to_texture_format(DXGI_FORMAT const format) -> TextureFormat {

    switch(format) {
        case DXGI_FORMAT_UNKNOWN: return TextureFormat::Unknown;
        case DXGI_FORMAT_R8G8B8A8_UNORM: return TextureFormat::RGBA8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return TextureFormat::BGRA8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_UNORM: return TextureFormat::BGR8_UNORM;
        case DXGI_FORMAT_R8_UNORM: return TextureFormat::R8_UNORM;
        case DXGI_FORMAT_BC1_UNORM: return TextureFormat::BC1;
        case DXGI_FORMAT_BC3_UNORM: return TextureFormat::BC3;
        case DXGI_FORMAT_BC4_UNORM: return TextureFormat::BC4;
        case DXGI_FORMAT_BC5_UNORM: return TextureFormat::BC5;
        case DXGI_FORMAT_R32_TYPELESS: return TextureFormat::D32_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return TextureFormat::RGBA16_FLOAT;
        default: return TextureFormat::Unknown;
    }
}

auto ionengine::renderer::rhi::texture_format_to_dxgi(TextureFormat const format) -> DXGI_FORMAT {

    switch(format) {
        case TextureFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
        case TextureFormat::RGBA8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::BGRA8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case TextureFormat::BGR8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM;
        case TextureFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case TextureFormat::BC1: return DXGI_FORMAT_BC1_UNORM;
        case TextureFormat::BC3: return DXGI_FORMAT_BC3_UNORM;
        case TextureFormat::BC4: return DXGI_FORMAT_BC4_UNORM;
        case TextureFormat::BC5: return DXGI_FORMAT_BC5_UNORM;
        case TextureFormat::D32_FLOAT: return DXGI_FORMAT_R32_TYPELESS;
        case TextureFormat::RGBA16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

DX12Texture::DX12Texture(ID3D12Device1* device, DescriptorAllocator& allocator, TextureUsageFlags const flags) {
    
}

DX12Texture::DX12Texture(
    ID3D12Device1* device, 
    winrt::com_ptr<ID3D12Resource> resource, 
    DescriptorAllocator& allocator, 
    TextureUsageFlags const flags
) : 
    descriptor_allocator(&allocator),
    resource(resource),
    flags(flags),
    resource_state(D3D12_RESOURCE_STATE_COMMON)
{
    auto d3d12_resource_desc = resource->GetDesc();
    width = static_cast<uint32_t>(d3d12_resource_desc.Width);
    height = d3d12_resource_desc.Height;
    depth = d3d12_resource_desc.DepthOrArraySize;
    mip_levels = d3d12_resource_desc.MipLevels;
    format = dxgi_to_texture_format(d3d12_resource_desc.Format);
    
    if(flags & TextureUsage::RenderTarget) {
        auto allocation = allocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
        if(!allocation.heap) {
            throw core::Exception("An error in descriptor allocation when creating a texture");
        }
        this->descriptor_allocation = allocation;

        auto d3d12_render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
        d3d12_render_target_view_desc.Format = d3d12_resource_desc.Format;
        d3d12_render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        device->CreateRenderTargetView(resource.get(), &d3d12_render_target_view_desc, allocation.cpu_handle());
    }
}

DX12Texture::~DX12Texture() {

    if(descriptor_allocation.heap) {
        descriptor_allocator->deallocate(descriptor_allocation);
    }
}
