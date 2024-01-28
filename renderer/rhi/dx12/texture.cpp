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

DX12Texture::DX12Texture(
    ID3D12Device1* device, 
    DX12MemoryAllocator& memory_allocator,
    DescriptorAllocator* descriptor_allocator,
    uint32_t const width,
    uint32_t const height,
    uint32_t const depth,
    uint32_t const mip_levels,
    TextureFormat const format,
    TextureDimension const dimension,
    TextureUsageFlags const flags
) :
    memory_allocator(&memory_allocator),
    descriptor_allocator(descriptor_allocator),
    width(width),
    height(height),
    depth(depth),
    mip_levels(mip_levels),
    format(format),
    dimension(dimension),
    flags(flags)
{
    auto d3d12_resource_desc = D3D12_RESOURCE_DESC {};
    switch(dimension) {
        case TextureDimension::_1D: {
            d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        } break;
        case TextureDimension::Cube:
        case TextureDimension::_2DArray:
        case TextureDimension::CubeArray:
        case TextureDimension::_2D: {
            d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        } break;
        case TextureDimension::_3D: {
            d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        } break;
    }
    d3d12_resource_desc.Width = width;
    d3d12_resource_desc.Height = height;
    d3d12_resource_desc.MipLevels = mip_levels;
    d3d12_resource_desc.DepthOrArraySize = depth;
    d3d12_resource_desc.SampleDesc.Count = 1;
    d3d12_resource_desc.Format = texture_format_to_dxgi(format);
    d3d12_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    if(flags & TextureUsage::DepthStencil) {
        d3d12_resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        resource_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }
    if(flags & TextureUsage::RenderTarget) {
        d3d12_resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        resource_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    if(flags & TextureUsage::UnorderedAccess) {
        d3d12_resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        resource_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }

    memory_allocation = this->memory_allocator->allocate(d3d12_resource_desc);
    if(!memory_allocation.heap) {
        throw core::Exception("An error in memory allocation when creating a buffer");
    }

    THROW_IF_FAILED(device->CreatePlacedResource(
        memory_allocation.heap, 
        memory_allocation.offset, 
        &d3d12_resource_desc, 
        resource_state,
        nullptr,
        __uuidof(ID3D12Resource),
        resource.put_void()
    ));

    if(flags & TextureUsage::RenderTarget) {
        auto allocation = descriptor_allocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
        if(!allocation.heap) {
            throw core::Exception("An error in descriptor allocation when creating a texture");
        }
        descriptor_allocations.emplace(TextureUsage::RenderTarget, allocation);

        auto d3d12_render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
        d3d12_render_target_view_desc.Format = d3d12_resource_desc.Format;
        d3d12_render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        device->CreateRenderTargetView(resource.get(), &d3d12_render_target_view_desc, allocation.cpu_handle());
    }

    if(flags & TextureUsage::ShaderResource) {

    }
}

DX12Texture::DX12Texture(
    ID3D12Device1* device, 
    winrt::com_ptr<ID3D12Resource> resource, 
    DescriptorAllocator& descriptor_allocator, 
    TextureUsageFlags const flags
) : 
    memory_allocator(nullptr),
    descriptor_allocator(&descriptor_allocator),
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
        auto allocation = descriptor_allocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
        if(!allocation.heap) {
            throw core::Exception("An error in descriptor allocation when creating a texture");
        }
        descriptor_allocations.emplace(TextureUsage::RenderTarget, allocation);

        auto d3d12_render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
        d3d12_render_target_view_desc.Format = d3d12_resource_desc.Format;
        d3d12_render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        device->CreateRenderTargetView(resource.get(), &d3d12_render_target_view_desc, allocation.cpu_handle());
    }
}

DX12Texture::~DX12Texture() {

    for(auto const& [usage, allocation] : descriptor_allocations) {
        descriptor_allocator->deallocate(allocation);
    }

    if(memory_allocator) {
        memory_allocator->deallocate(memory_allocation);
    }
}
