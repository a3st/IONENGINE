// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "buffer.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

DX12Buffer::DX12Buffer(
    ID3D12Device1* device, 
    MemoryAllocator& memory_allocator,
    DescriptorAllocator* descriptor_allocator,
    size_t const size,
    BufferUsageFlags const flags
) :
    memory_allocator(static_cast<DX12MemoryAllocator*>(&memory_allocator)),
    descriptor_allocator(descriptor_allocator),
    size(size),
    flags(flags)
{
    auto d3d12_resource_desc = D3D12_RESOURCE_DESC {};
    d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    d3d12_resource_desc.Width = size;
    d3d12_resource_desc.Height = 1;
    d3d12_resource_desc.MipLevels = 1;
    d3d12_resource_desc.DepthOrArraySize = 1;
    d3d12_resource_desc.SampleDesc.Count = 1;
    d3d12_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    d3d12_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    memory_allocation = this->memory_allocator->allocate(d3d12_resource_desc);
    if(!memory_allocation.heap) {
        throw core::Exception("An error in memory allocation when creating a buffer");
    }

    if(flags & BufferUsage::MapWrite) {
        resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else if(flags & BufferUsage::MapRead) {
        resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else {
        resource_state = D3D12_RESOURCE_STATE_COMMON;
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

    if(flags & BufferUsage::ConstantBuffer) {
        assert(!descriptor_allocator && "To create a buffer with views, you need to pass the allocator descriptor");

        auto allocation = descriptor_allocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
        if(!allocation.heap) {
            throw core::Exception("An error in descriptor allocation when creating a buffer");
        }
        this->descriptor_allocation = allocation;

        auto d3d12_constant_buffer_view_desc = D3D12_CONSTANT_BUFFER_VIEW_DESC {};
        d3d12_constant_buffer_view_desc.BufferLocation = resource->GetGPUVirtualAddress();
        d3d12_constant_buffer_view_desc.SizeInBytes = static_cast<uint32_t>(size);

        device->CreateConstantBufferView(&d3d12_constant_buffer_view_desc, allocation.cpu_handle());
    }
}

DX12Buffer::~DX12Buffer() {

    if(descriptor_allocator && descriptor_allocation.heap) {
        descriptor_allocator->deallocate(descriptor_allocation);
    }
}