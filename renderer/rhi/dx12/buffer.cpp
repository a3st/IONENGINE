// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "buffer.hpp"
#include "core/exception.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    DX12Buffer::DX12Buffer(ID3D12Device1* device, DX12MemoryAllocator& memory_allocator,
                           DescriptorAllocator* descriptor_allocator, size_t const size, BufferUsageFlags const flags)
        : memory_allocator(&memory_allocator), descriptor_allocator(descriptor_allocator), size(size), flags(flags),
          resource_state(D3D12_RESOURCE_STATE_COMMON)
    {
        auto d3d12_resource_desc = D3D12_RESOURCE_DESC{};
        d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        if (flags & BufferUsage::ConstantBuffer)
        {
            d3d12_resource_desc.Width =
                static_cast<uint64_t>((static_cast<uint32_t>(size) + (DX12_CONSTANT_BUFFER_SIZE_ALIGNMENT - 1)) &
                                      ~(DX12_CONSTANT_BUFFER_SIZE_ALIGNMENT - 1));
        }
        else
        {
            d3d12_resource_desc.Width = size;
        }
        d3d12_resource_desc.Height = 1;
        d3d12_resource_desc.MipLevels = 1;
        d3d12_resource_desc.DepthOrArraySize = 1;
        d3d12_resource_desc.SampleDesc.Count = 1;
        d3d12_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        d3d12_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        memory_allocation = this->memory_allocator->allocate(d3d12_resource_desc);
        if (memory_allocation.size == 0)
        {
            throw core::Exception("An error in memory allocation when creating a buffer");
        }

        if (flags & BufferUsage::MapWrite)
        {
            assert(memory_allocator.get_flags() == MemoryAllocatorUsage::Upload &&
                   "Buffer with MapWrite flag can be allocated with Upload memory allocator");
            resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
        }
        else if (flags & BufferUsage::MapRead)
        {
            resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
        }

        THROW_IF_FAILED(device->CreatePlacedResource(memory_allocation.heap, memory_allocation.offset,
                                                     &d3d12_resource_desc, resource_state, nullptr,
                                                     __uuidof(ID3D12Resource), resource.put_void()));

        if (flags & BufferUsage::ConstantBuffer)
        {
            assert(descriptor_allocator && "To create a buffer with views, you need to pass the allocator descriptor");

            auto allocation = descriptor_allocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a buffer");
            }
            descriptor_allocations.emplace(BufferUsage::ConstantBuffer, allocation);

            auto d3d12_constant_buffer_view_desc = D3D12_CONSTANT_BUFFER_VIEW_DESC{};
            d3d12_constant_buffer_view_desc.BufferLocation = resource->GetGPUVirtualAddress();
            d3d12_constant_buffer_view_desc.SizeInBytes = static_cast<uint32_t>(d3d12_resource_desc.Width);

            device->CreateConstantBufferView(&d3d12_constant_buffer_view_desc, allocation.cpu_handle());
        }
    }

    DX12Buffer::~DX12Buffer()
    {
        if (descriptor_allocator)
        {
            for (auto const& [usage, allocation] : descriptor_allocations)
            {
                descriptor_allocator->deallocate(allocation);
            }
        }

        memory_allocator->deallocate(memory_allocation);
    }
} // namespace ionengine::renderer::rhi