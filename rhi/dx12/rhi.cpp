// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "rhi.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine::rhi
{
    DescriptorAllocator::DescriptorAllocator(ID3D12Device1* device) : device(device)
    {
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }

    auto DescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size)
        -> DescriptorAllocation
    {
        assert((chunks.find(heap_type) != chunks.end()) && "Required heap not found when allocating descriptor");

        auto allocation = DescriptorAllocation{};
        auto& chunk = chunks[heap_type];

        if (size > chunk.size - chunk.offset)
        {
            throw core::Exception("Required heap does not contain free descriptors");
        }

        uint32_t alloc_start = chunk.offset;
        uint32_t alloc_size = 0;
        bool success = false;

        for (uint32_t const i : std::views::iota(chunk.offset, chunk.size))
        {
            if (alloc_size == size)
            {
                success = true;
                break;
            }

            if (chunk.free[i] == 1)
            {
                alloc_size++;
            }
            else
            {
                alloc_start = i + 1;
                alloc_size = 0;
            }
        }

        if (success)
        {
            std::fill(chunk.free.begin() + alloc_start, chunk.free.begin() + alloc_start + alloc_size, 0);
            chunk.offset += alloc_size;

            allocation = {.heap = chunk.heap.get(),
                          .heap_type = heap_type,
                          .increment_size = device->GetDescriptorHandleIncrementSize(heap_type),
                          .offset = alloc_start,
                          .size = size};
        }
        else
        {
            throw core::Exception("Required heap does not contain free descriptors");
        }
        return allocation;
    }

    auto DescriptorAllocator::deallocate(DescriptorAllocation const& allocation) -> void
    {
        assert((chunks.find(allocation.heap_type) != chunks.end()) &&
               "Required heap not found when deallocating descriptor");

        auto& chunk = chunks[allocation.heap_type];
        std::fill(chunk.free.begin() + allocation.offset, chunk.free.begin() + allocation.offset + allocation.size, 1);
        chunk.offset = allocation.offset;
    }

    auto DescriptorAllocator::create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type) -> void
    {
        uint32_t alloc_size = 0;
        D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        switch (heap_type)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_CBV_SRV_UAV_MAX;
                flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_RTV_MAX;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_DSV_MAX;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_SAMPLER_MAX;
                flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                break;
            }
        }

        auto d3d12_descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC{};
        d3d12_descriptor_heap_desc.NumDescriptors = alloc_size;
        d3d12_descriptor_heap_desc.Type = heap_type;
        d3d12_descriptor_heap_desc.Flags = flags;

        winrt::com_ptr<ID3D12DescriptorHeap> descriptor_heap;
        THROW_IF_FAILED(device->CreateDescriptorHeap(&d3d12_descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
                                                     descriptor_heap.put_void()));

        auto chunk = Chunk{.heap = descriptor_heap, .offset = 0, .size = alloc_size};
        chunk.free.resize(alloc_size, 1);
        chunks.emplace(heap_type, chunk);
    }

    MemoryAllocator::MemoryAllocator(ID3D12Device1* device, size_t const block_size, size_t const chunk_size)
        : device(device), block_size(block_size), chunk_size(chunk_size)
    {
        assert((chunk_size % block_size == 0) && "Chunk size must be completely divisible by block size");
        assert((chunk_size < DX12_MEMORY_ALLOCATOR_CHUNK_MAX_SIZE) &&
               "Chunk size must be less than DX12_MEMORY_ALLOCATOR_CHUNK_MAX_SIZE");
    }

    auto MemoryAllocator::allocate(D3D12_RESOURCE_DESC const& resource_desc, D3D12_HEAP_TYPE const heap_type)
        -> MemoryAllocation
    {
        auto allocation_info = device->GetResourceAllocationInfo(0, 1, &resource_desc);
        size_t const size = (allocation_info.SizeInBytes + (block_size - 1)) & ~(block_size - 1);

        std::lock_guard lock(mutex);

        chunks.try_emplace({allocation_info.Alignment, heap_type});

        auto allocation = MemoryAllocation{};

        for (auto& chunk : chunks[{allocation_info.Alignment, heap_type}])
        {
            if (size > chunk.size - chunk.offset)
            {
                continue;
            }

            uint64_t alloc_start = chunk.offset;
            size_t alloc_size = 0;
            bool success = false;

            for (uint64_t i = chunk.offset; i < chunk.size; i += block_size)
            {
                if (alloc_size >= allocation_info.SizeInBytes)
                {
                    success = true;
                    break;
                }

                if (chunk.free[i / block_size] == 1)
                {
                    alloc_size += block_size;
                }
                else
                {
                    alloc_start = i + block_size;
                    alloc_size = 0;
                }
            }

            if (success)
            {
                std::fill(chunk.free.begin() + (alloc_start / block_size),
                          chunk.free.begin() + (alloc_start / block_size) + (alloc_size / block_size), 0);
                chunk.offset += alloc_size;

                allocation = {.heap = chunk.heap.get(),
                              .alignment = allocation_info.Alignment,
                              .offset = alloc_start,
                              .size = alloc_size};
                break;
            }
        }

        if (allocation.size == 0)
        {
            create_chunk(allocation_info.Alignment, heap_type);

            auto& chunk = chunks[{allocation_info.Alignment, heap_type}].back();
            uint64_t const alloc_start = chunk.offset;
            size_t const alloc_size = size;

            std::fill(chunk.free.begin() + (alloc_start / block_size),
                      chunk.free.begin() + (alloc_start / block_size) + (alloc_size / block_size), 0);
            chunk.offset += alloc_size;

            allocation = {.heap = chunk.heap.get(),
                          .alignment = allocation_info.Alignment,
                          .offset = alloc_start,
                          .size = alloc_size};
        }
        return allocation;
    }

    auto MemoryAllocator::deallocate(MemoryAllocation const& allocation) -> void
    {
        std::lock_guard lock(mutex);

        assert((ptr_chunks.find((uintptr_t)allocation.heap) != ptr_chunks.end()) &&
               "Required chunk not found when deallocating descriptor");

        uint32_t const chunk_index = ptr_chunks[(uintptr_t)allocation.heap];
        auto& chunk = chunks[{allocation.alignment, allocation.heap->GetDesc().Properties.Type}][chunk_index];

        std::fill(chunk.free.begin() + (allocation.offset / block_size),
                  chunk.free.begin() + (allocation.offset / block_size) + (allocation.size / block_size), 1);
        chunk.offset = allocation.offset;
    }

    auto MemoryAllocator::reset() -> void
    {
        assert(false && "MemoryAllocator doesn't support reset method");
    }

    auto MemoryAllocator::create_chunk(uint64_t const alignment, D3D12_HEAP_TYPE const heap_type) -> void
    {
        auto d3d12_heap_desc = D3D12_HEAP_DESC{};
        d3d12_heap_desc.Alignment = alignment;
        d3d12_heap_desc.SizeInBytes = chunk_size;
        d3d12_heap_desc.Properties.Type = heap_type;

        winrt::com_ptr<ID3D12Heap> heap;
        THROW_IF_FAILED(device->CreateHeap(&d3d12_heap_desc, __uuidof(ID3D12Heap), heap.put_void()));

        auto chunk = Chunk{.heap = heap, .offset = 0, .size = chunk_size};
        chunk.free.resize(chunk_size / block_size, 1);
        chunks[{alignment, heap_type}].emplace_back(std::move(chunk));

        ptr_chunks.emplace((uintptr_t)heap.get(), static_cast<uint32_t>(chunks[{alignment, heap_type}].size() - 1));
    }

    DX12Buffer::DX12Buffer(ID3D12Device1* device, MemoryAllocator& memory_allocator,
                           DescriptorAllocator* descriptor_allocator, size_t const size, size_t const element_stride,
                           BufferUsageFlags const flags)
        : memory_allocator(&memory_allocator), descriptor_allocator(descriptor_allocator), size(size), flags(flags)
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

        D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_COMMON;
        D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT;

        if (flags & BufferUsage::MapWrite)
        {
            initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
            heap_type = D3D12_HEAP_TYPE_UPLOAD;
        }
        else if (flags & BufferUsage::MapRead)
        {
            initial_state = D3D12_RESOURCE_STATE_COPY_DEST;
            heap_type = D3D12_HEAP_TYPE_READBACK;
        }

        memory_allocation = this->memory_allocator->allocate(d3d12_resource_desc, heap_type);
        if (memory_allocation.size == 0)
        {
            throw core::Exception("An error in memory allocation when creating a buffer");
        }

        THROW_IF_FAILED(device->CreatePlacedResource(memory_allocation.heap, memory_allocation.offset,
                                                     &d3d12_resource_desc, initial_state, nullptr,
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
        if (flags & BufferUsage::ShaderResource)
        {
            assert(descriptor_allocator && "To create a buffer with views, you need to pass the allocator descriptor");

            auto allocation = descriptor_allocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a buffer");
            }
            descriptor_allocations.emplace(BufferUsage::ShaderResource, allocation);

            auto d3d12_shader_resource_view_desc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
            d3d12_shader_resource_view_desc.Buffer.FirstElement = 0;
            d3d12_shader_resource_view_desc.Buffer.NumElements = size / element_stride;
            d3d12_shader_resource_view_desc.Buffer.StructureByteStride = element_stride;

            device->CreateShaderResourceView(resource.get(), &d3d12_shader_resource_view_desc, allocation.cpu_handle());
        }
        if (flags & BufferUsage::UnorderedAccess)
        {
            assert(descriptor_allocator && "To create a buffer with views, you need to pass the allocator descriptor");

            auto allocation = descriptor_allocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a buffer");
            }
            descriptor_allocations.emplace(BufferUsage::UnorderedAccess, allocation);

            auto d3d12_unordered_access_view_desc = D3D12_UNORDERED_ACCESS_VIEW_DESC{};
            d3d12_unordered_access_view_desc.Buffer.FirstElement = 0;
            d3d12_unordered_access_view_desc.Buffer.NumElements = size / element_stride;
            d3d12_unordered_access_view_desc.Buffer.StructureByteStride = element_stride;

            device->CreateUnorderedAccessView(resource.get(), nullptr, &d3d12_unordered_access_view_desc,
                                              allocation.cpu_handle());
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

    auto DX12Buffer::map_memory() -> uint8_t*
    {
        uint8_t* mapped_bytes;
        if (flags & BufferUsage::MapWrite)
        {
            THROW_IF_FAILED(resource->Map(0, nullptr, reinterpret_cast<void**>(&mapped_bytes)));
        }
        else
        {
            auto range = D3D12_RANGE{.Begin = 0, .End = size};
            THROW_IF_FAILED(resource->Map(0, &range, reinterpret_cast<void**>(&mapped_bytes)));
        }
        return mapped_bytes;
    }

    auto DX12Buffer::unmap_memory() -> void
    {
        if (flags & BufferUsage::MapWrite)
        {
            auto range = D3D12_RANGE{.Begin = 0, .End = size};
            resource->Unmap(0, &range);
        }
        else if (flags & BufferUsage::MapRead)
        {
            resource->Unmap(0, nullptr);
        }
    }

    auto dxgi_to_texture_format(DXGI_FORMAT const format) -> TextureFormat
    {
        switch (format)
        {
            case DXGI_FORMAT_UNKNOWN:
                return TextureFormat::Unknown;
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                return TextureFormat::RGBA8_UNORM;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                return TextureFormat::BGRA8_UNORM;
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                return TextureFormat::BGR8_UNORM;
            case DXGI_FORMAT_R8_UNORM:
                return TextureFormat::R8_UNORM;
            case DXGI_FORMAT_BC1_UNORM:
                return TextureFormat::BC1;
            case DXGI_FORMAT_BC3_UNORM:
                return TextureFormat::BC3;
            case DXGI_FORMAT_BC4_UNORM:
                return TextureFormat::BC4;
            case DXGI_FORMAT_BC5_UNORM:
                return TextureFormat::BC5;
            case DXGI_FORMAT_D32_FLOAT:
                return TextureFormat::D32_FLOAT;
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return TextureFormat::RGBA16_FLOAT;
            default:
                return TextureFormat::Unknown;
        }
    }

    auto texture_format_to_dxgi(TextureFormat const format) -> DXGI_FORMAT
    {
        switch (format)
        {
            case TextureFormat::Unknown:
                return DXGI_FORMAT_UNKNOWN;
            case TextureFormat::RGBA8_UNORM:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::BGRA8_UNORM:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::BGR8_UNORM:
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            case TextureFormat::R8_UNORM:
                return DXGI_FORMAT_R8_UNORM;
            case TextureFormat::BC1:
                return DXGI_FORMAT_BC1_UNORM;
            case TextureFormat::BC3:
                return DXGI_FORMAT_BC3_UNORM;
            case TextureFormat::BC4:
                return DXGI_FORMAT_BC4_UNORM;
            case TextureFormat::BC5:
                return DXGI_FORMAT_BC5_UNORM;
            case TextureFormat::D32_FLOAT:
                return DXGI_FORMAT_D32_FLOAT;
            case TextureFormat::RGBA16_FLOAT:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    DX12Texture::DX12Texture(ID3D12Device1* device, MemoryAllocator& memory_allocator,
                             DescriptorAllocator& descriptor_allocator, uint32_t const width, uint32_t const height,
                             uint32_t const depth, uint32_t const mip_levels, TextureFormat const format,
                             TextureDimension const dimension, TextureUsageFlags const flags)
        : memory_allocator(&memory_allocator), descriptor_allocator(&descriptor_allocator), width(width),
          height(height), depth(depth), mip_levels(mip_levels), format(format), dimension(dimension), flags(flags)
    {
        auto d3d12_resource_desc = D3D12_RESOURCE_DESC{};
        switch (dimension)
        {
            case TextureDimension::_1D: {
                d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
                break;
            }
            case TextureDimension::Cube:
            case TextureDimension::_2DArray:
            case TextureDimension::CubeArray:
            case TextureDimension::_2D: {
                d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                break;
            }
            case TextureDimension::_3D: {
                d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
                break;
            }
        }
        d3d12_resource_desc.Width = width;
        d3d12_resource_desc.Height = height;
        d3d12_resource_desc.MipLevels = mip_levels;
        d3d12_resource_desc.DepthOrArraySize = depth;
        d3d12_resource_desc.SampleDesc.Count = 1;
        d3d12_resource_desc.Format = texture_format_to_dxgi(format);
        d3d12_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_COMMON;
        D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT;

        if (flags & TextureUsage::DepthStencil)
        {
            d3d12_resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            initial_state = D3D12_RESOURCE_STATE_DEPTH_READ;
        }
        if (flags & TextureUsage::RenderTarget)
        {
            d3d12_resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }
        if (flags & TextureUsage::UnorderedAccess)
        {
            d3d12_resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        memory_allocation = this->memory_allocator->allocate(d3d12_resource_desc, heap_type);
        if (memory_allocation.size == 0)
        {
            throw core::Exception("An error in memory allocation when creating a texture");
        }

        THROW_IF_FAILED(device->CreatePlacedResource(memory_allocation.heap, memory_allocation.offset,
                                                     &d3d12_resource_desc, initial_state, nullptr,
                                                     __uuidof(ID3D12Resource), resource.put_void()));

        if (flags & TextureUsage::RenderTarget)
        {
            auto allocation = descriptor_allocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptor_allocations.emplace(TextureUsage::RenderTarget, allocation);

            auto d3d12_render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC{};
            d3d12_render_target_view_desc.Format = d3d12_resource_desc.Format;
            d3d12_render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            device->CreateRenderTargetView(resource.get(), &d3d12_render_target_view_desc, allocation.cpu_handle());
        }

        if (flags & TextureUsage::DepthStencil)
        {
            auto allocation = descriptor_allocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptor_allocations.emplace(TextureUsage::DepthStencil, allocation);

            auto d3d12_depth_stencil_view = D3D12_DEPTH_STENCIL_VIEW_DESC{};
            d3d12_depth_stencil_view.Format = DXGI_FORMAT_D32_FLOAT;
            switch (dimension)
            {
                case TextureDimension::_2D: {
                    d3d12_depth_stencil_view.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    break;
                }
                default: {
                    assert(false && "Creation of depth stencil only possible in 2D dimension");
                    break;
                }
            }

            device->CreateDepthStencilView(resource.get(), &d3d12_depth_stencil_view, allocation.cpu_handle());
        }

        if (flags & TextureUsage::ShaderResource)
        {
            auto allocation = descriptor_allocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptor_allocations.emplace(TextureUsage::ShaderResource, allocation);

            auto d3d12_shader_resource_view = D3D12_SHADER_RESOURCE_VIEW_DESC{};
            d3d12_shader_resource_view.Format = d3d12_resource_desc.Format;
            d3d12_shader_resource_view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            switch (dimension)
            {
                case TextureDimension::_1D: {
                    d3d12_shader_resource_view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                    d3d12_shader_resource_view.Texture1D.MipLevels = mip_levels;
                    break;
                }
                case TextureDimension::_2D: {
                    d3d12_shader_resource_view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    d3d12_shader_resource_view.Texture2D.MipLevels = mip_levels;
                    break;
                }
                case TextureDimension::_3D: {
                    d3d12_shader_resource_view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                    d3d12_shader_resource_view.Texture3D.MipLevels = mip_levels;
                    break;
                }
                case TextureDimension::Cube: {
                    d3d12_shader_resource_view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    d3d12_shader_resource_view.Texture3D.MipLevels = mip_levels;
                    break;
                }
            }

            device->CreateShaderResourceView(resource.get(), &d3d12_shader_resource_view, allocation.cpu_handle());
        }
    }

    DX12Texture::DX12Texture(ID3D12Device1* device, winrt::com_ptr<ID3D12Resource> resource,
                             DescriptorAllocator& descriptor_allocator, TextureUsageFlags const flags)
        : memory_allocator(nullptr), descriptor_allocator(&descriptor_allocator), resource(resource), flags(flags)
    {
        auto d3d12_resource_desc = resource->GetDesc();
        width = static_cast<uint32_t>(d3d12_resource_desc.Width);
        height = d3d12_resource_desc.Height;
        depth = d3d12_resource_desc.DepthOrArraySize;
        mip_levels = d3d12_resource_desc.MipLevels;
        format = dxgi_to_texture_format(d3d12_resource_desc.Format);

        if (flags & TextureUsage::RenderTarget)
        {
            auto allocation = descriptor_allocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptor_allocations.emplace(TextureUsage::RenderTarget, allocation);

            auto d3d12_render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC{};
            d3d12_render_target_view_desc.Format = d3d12_resource_desc.Format;
            d3d12_render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            device->CreateRenderTargetView(resource.get(), &d3d12_render_target_view_desc, allocation.cpu_handle());
        }
    }

    DX12Texture::~DX12Texture()
    {
        for (auto const& [usage, allocation] : descriptor_allocations)
        {
            descriptor_allocator->deallocate(allocation);
        }

        if (memory_allocator)
        {
            memory_allocator->deallocate(memory_allocation);
        }
    }

    auto element_type_to_dxgi(shader_file::ElementType const element_type) -> DXGI_FORMAT
    {
        switch (element_type)
        {
            case shader_file::ElementType::Float4:
            case shader_file::ElementType::Float4x4:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case shader_file::ElementType::Float3:
            case shader_file::ElementType::Float3x3:
                return DXGI_FORMAT_R32G32B32_FLOAT;
            case shader_file::ElementType::Float2:
            case shader_file::ElementType::Float2x2:
                return DXGI_FORMAT_R32G32_FLOAT;
            case shader_file::ElementType::Float:
                return DXGI_FORMAT_R32_FLOAT;
            case shader_file::ElementType::Uint:
                return DXGI_FORMAT_R32_UINT;
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    auto element_type_size(rhi::shader_file::ElementType const element_type) -> uint32_t
    {
        switch (element_type)
        {
            case shader_file::ElementType::Float4:
                return sizeof(float) * 4;
            case shader_file::ElementType::Float4x4:
                return sizeof(float) * 16;
            case shader_file::ElementType::Float3:
                return sizeof(float) * 3;
            case shader_file::ElementType::Float3x3:
                return sizeof(float) * 9;
            case shader_file::ElementType::Float2:
                return sizeof(float) * 2;
            case shader_file::ElementType::Float2x2:
                return sizeof(float) * 4;
            case shader_file::ElementType::Float:
                return sizeof(float);
            case shader_file::ElementType::Uint:
                return sizeof(uint32_t);
            default:
                return 0;
        }
    }

    DX12Shader::DX12Shader(ID3D12Device4* device, std::span<uint8_t const> const data)
    {
        shader_file::ShaderFile shader_file(data);

        if (shader_file.get_flags() != shader_file::ShaderFileFlags::DXIL)
        {
            throw core::Exception("Unknown shader binary data");
        }

        shader_name = shader_file.get_name();

        XXHash64 hasher(0);
        for (auto const& [stage, data] : shader_file.get_stages())
        {
            std::vector<uint8_t> buffer;
            {
                auto shader_bytes = shader_file.get_buffer(data.buffer);
                buffer.resize(shader_bytes.size());
                std::memcpy(buffer.data(), shader_bytes.data(), buffer.size());
            }
            hasher.add(buffer.data(), buffer.size());
            buffers.emplace_back(std::move(buffer));

            auto d3d12_shader_bytecode = D3D12_SHADER_BYTECODE{};
            d3d12_shader_bytecode.pShaderBytecode = buffers.back().data();
            d3d12_shader_bytecode.BytecodeLength = buffers.back().size();

            stages.emplace(stage, d3d12_shader_bytecode);
        }
        hash = hasher.hash();

        {
            auto result = shader_file.get_stages().find(shader_file::ShaderStageType::Vertex);
            if (result != shader_file.get_stages().end())
            {
                uint32_t offset = 0;
                for (auto const& input : result->second.inputs)
                {
                    uint32_t index = 0;
                    if (std::isdigit(input.semantic.back()) != 0)
                    {
                        index = (int32_t)input.semantic.back() - 48;
                        semantic_names.emplace_back(input.semantic.substr(0, input.semantic.size() - 1));
                    }
                    else
                    {
                        semantic_names.emplace_back(input.semantic);
                    }

                    auto d3d12_input_element = D3D12_INPUT_ELEMENT_DESC{};
                    d3d12_input_element.SemanticName = semantic_names.back().c_str();
                    d3d12_input_element.SemanticIndex = index;
                    d3d12_input_element.Format = element_type_to_dxgi(input.element_type);
                    d3d12_input_element.InputSlot = 0;
                    d3d12_input_element.AlignedByteOffset = offset;
                    d3d12_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    d3d12_input_element.InstanceDataStepRate = 0;
                    inputs.emplace_back(d3d12_input_element);

                    offset += element_type_size(input.element_type);
                }
                inputs_size_per_vertex = result->second.inputs_size_per_vertex;
            }
        }

        bindings = shader_file.get_exports();
    }

    auto DX12Shader::get_name() const -> std::string_view
    {
        return shader_name;
    }

    auto DX12Shader::get_inputs() const -> std::span<D3D12_INPUT_ELEMENT_DESC const>
    {
        return inputs;
    }

    auto DX12Shader::get_inputs_size_per_vertex() const -> uint32_t
    {
        return inputs_size_per_vertex;
    }

    auto DX12Shader::get_stages() const
        -> std::unordered_map<shader_file::ShaderStageType, D3D12_SHADER_BYTECODE> const&
    {
        return stages;
    }

    auto DX12Shader::get_bindings() const -> std::unordered_map<std::string, shader_file::ResourceData> const&
    {
        return bindings;
    }

    auto DX12Shader::get_hash() const -> uint64_t
    {
        return hash;
    }

    auto compare_op_to_d3d12(CompareOp const compare_op) -> D3D12_COMPARISON_FUNC
    {
        switch (compare_op)
        {
            case CompareOp::Always:
                return D3D12_COMPARISON_FUNC_ALWAYS;
            case CompareOp::Equal:
                return D3D12_COMPARISON_FUNC_EQUAL;
            case CompareOp::Greater:
                return D3D12_COMPARISON_FUNC_GREATER;
            case CompareOp::GreaterEqual:
                return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case CompareOp::Less:
                return D3D12_COMPARISON_FUNC_LESS;
            case CompareOp::LessEqual:
                return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case CompareOp::Never:
                return D3D12_COMPARISON_FUNC_NEVER;
            case CompareOp::NotEqual:
                return D3D12_COMPARISON_FUNC_NOT_EQUAL;
            default:
                return D3D12_COMPARISON_FUNC_NEVER;
        }
    }

    auto blend_to_d3d12(Blend const blend) -> D3D12_BLEND
    {
        switch (blend)
        {
            case Blend::InvSrcAlpha:
                return D3D12_BLEND_INV_SRC_ALPHA;
            case Blend::One:
                return D3D12_BLEND_ONE;
            case Blend::SrcAlpha:
                return D3D12_BLEND_SRC_ALPHA;
            case Blend::Zero:
                return D3D12_BLEND_ZERO;
            case Blend::BlendFactor:
                return D3D12_BLEND_BLEND_FACTOR;
            default:
                return D3D12_BLEND_ZERO;
        }
    }

    auto blend_op_to_d3d12(BlendOp const blend_op) -> D3D12_BLEND_OP
    {
        switch (blend_op)
        {
            case BlendOp::Add:
                return D3D12_BLEND_OP_ADD;
            case BlendOp::Max:
                return D3D12_BLEND_OP_MAX;
            case BlendOp::Min:
                return D3D12_BLEND_OP_MIN;
            case BlendOp::RevSubtract:
                return D3D12_BLEND_OP_REV_SUBTRACT;
            case BlendOp::Subtract:
                return D3D12_BLEND_OP_SUBTRACT;
            default:
                return D3D12_BLEND_OP_ADD;
        }
    }

    Pipeline::Pipeline(ID3D12Device4* device, ID3D12RootSignature* root_signature, DX12Shader& shader,
                       RasterizerStageInfo const& rasterizer, BlendColorInfo const& blend_color,
                       std::optional<DepthStencilStageInfo> const depth_stencil,
                       std::span<DXGI_FORMAT const> const render_target_formats, DXGI_FORMAT const depth_stencil_format,
                       ID3DBlob* blob)
        : root_signature(root_signature)
    {
        auto d3d12_pipeline_desc = D3D12_GRAPHICS_PIPELINE_STATE_DESC{};
        d3d12_pipeline_desc.pRootSignature = root_signature;
        for (auto const& [stage, data] : shader.get_stages())
        {
            switch (stage)
            {
                case shader_file::ShaderStageType::Vertex: {
                    d3d12_pipeline_desc.VS = data;

                    auto d3d12_input_layout = D3D12_INPUT_LAYOUT_DESC{};
                    d3d12_input_layout.pInputElementDescs = shader.get_inputs().data();
                    d3d12_input_layout.NumElements = static_cast<uint32_t>(shader.get_inputs().size());

                    d3d12_pipeline_desc.InputLayout = d3d12_input_layout;
                    break;
                }
                case shader_file::ShaderStageType::Pixel: {
                    d3d12_pipeline_desc.PS = data;

                    auto d3d12_render_target_blend = D3D12_RENDER_TARGET_BLEND_DESC{};
                    d3d12_render_target_blend.BlendEnable = blend_color.blend_enable;
                    d3d12_render_target_blend.SrcBlend = blend_to_d3d12(blend_color.blend_src);
                    d3d12_render_target_blend.DestBlend = blend_to_d3d12(blend_color.blend_dst);
                    d3d12_render_target_blend.BlendOp = blend_op_to_d3d12(blend_color.blend_op);
                    d3d12_render_target_blend.SrcBlendAlpha = blend_to_d3d12(blend_color.blend_src_alpha);
                    d3d12_render_target_blend.DestBlendAlpha = blend_to_d3d12(blend_color.blend_dst_alpha);
                    d3d12_render_target_blend.BlendOpAlpha = blend_op_to_d3d12(blend_color.blend_op_alpha);
                    d3d12_render_target_blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

                    auto d3d12_blend = D3D12_BLEND_DESC{};
                    uint32_t render_target_count = 0;
                    for (uint32_t const i : std::views::iota(0u, render_target_formats.size()))
                    {
                        if (render_target_formats[i] == DXGI_FORMAT_UNKNOWN)
                        {
                            break;
                        }

                        d3d12_pipeline_desc.RTVFormats[i] = render_target_formats[i];
                        d3d12_blend.RenderTarget[i] = d3d12_render_target_blend;

                        render_target_count++;
                    }
                    d3d12_pipeline_desc.NumRenderTargets = static_cast<uint32_t>(render_target_formats.size());

                    d3d12_pipeline_desc.BlendState = d3d12_blend;

                    d3d12_pipeline_desc.DSVFormat = depth_stencil_format;
                    break;
                }
            }
        }

        auto d3d12_rasterizer = D3D12_RASTERIZER_DESC{};
        switch (rasterizer.fill_mode)
        {
            case FillMode::Solid: {
                d3d12_rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
                break;
            }
            case FillMode::Wireframe: {
                d3d12_rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
                break;
            }
        }
        switch (rasterizer.cull_mode)
        {
            case CullMode::Back: {
                d3d12_rasterizer.CullMode = D3D12_CULL_MODE_BACK;
                break;
            }
            case CullMode::Front: {
                d3d12_rasterizer.CullMode = D3D12_CULL_MODE_FRONT;
                break;
            }
            case CullMode::None: {
                d3d12_rasterizer.CullMode = D3D12_CULL_MODE_NONE;
                break;
            }
        }
        d3d12_rasterizer.FrontCounterClockwise = true;
        d3d12_rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        d3d12_rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        d3d12_rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        d3d12_rasterizer.DepthClipEnable = true;
        d3d12_rasterizer.MultisampleEnable = false;
        d3d12_rasterizer.AntialiasedLineEnable = false;
        d3d12_rasterizer.ForcedSampleCount = 0;
        d3d12_rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        d3d12_pipeline_desc.RasterizerState = d3d12_rasterizer;

        d3d12_pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        d3d12_pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();
        d3d12_pipeline_desc.SampleDesc.Count = 1;

        auto d3d12_depth_stencil_face = D3D12_DEPTH_STENCILOP_DESC{};
        d3d12_depth_stencil_face.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        d3d12_depth_stencil_face.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        d3d12_depth_stencil_face.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        d3d12_depth_stencil_face.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        auto d3d12_depth_stencil = D3D12_DEPTH_STENCIL_DESC{};
        auto depth_stencil_value = depth_stencil.value_or(DepthStencilStageInfo::Default());
        d3d12_depth_stencil.DepthFunc = compare_op_to_d3d12(depth_stencil_value.depth_func);
        d3d12_depth_stencil.DepthEnable = depth_stencil_value.depth_write;
        d3d12_depth_stencil.StencilEnable = depth_stencil_value.stencil_write;
        d3d12_depth_stencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        d3d12_depth_stencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        d3d12_depth_stencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        d3d12_depth_stencil.FrontFace = d3d12_depth_stencil_face;
        d3d12_depth_stencil.BackFace = d3d12_depth_stencil_face;

        d3d12_pipeline_desc.DepthStencilState = d3d12_depth_stencil;

        if (blob)
        {
            d3d12_pipeline_desc.CachedPSO.pCachedBlob = blob->GetBufferPointer();
            d3d12_pipeline_desc.CachedPSO.CachedBlobSizeInBytes = blob->GetBufferSize();
        }

        HRESULT result = device->CreateGraphicsPipelineState(&d3d12_pipeline_desc, __uuidof(ID3D12PipelineState),
                                                             pipeline_state.put_void());
        if (result == D3D12_ERROR_ADAPTER_NOT_FOUND | result == D3D12_ERROR_DRIVER_VERSION_MISMATCH |
            result == E_INVALIDARG)
        {
            d3d12_pipeline_desc.CachedPSO = {};
            THROW_IF_FAILED(device->CreateGraphicsPipelineState(&d3d12_pipeline_desc, __uuidof(ID3D12PipelineState),
                                                                pipeline_state.put_void()));
        }
        else
        {
            THROW_IF_FAILED(result);
        }
    }

    PipelineCache::PipelineCache(ID3D12Device4* device) : device(device)
    {
        D3D12_ROOT_PARAMETER1 d3d12_root_parameter = {};
        d3d12_root_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        d3d12_root_parameter.Constants.ShaderRegister = 0;
        d3d12_root_parameter.Constants.RegisterSpace = 0;
        d3d12_root_parameter.Constants.Num32BitValues = 16;
        d3d12_root_parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        auto d3d12_static_sampler = D3D12_STATIC_SAMPLER_DESC{};
        d3d12_static_sampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        d3d12_static_sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        d3d12_static_sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        d3d12_static_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        d3d12_static_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        d3d12_static_sampler.MaxAnisotropy = 2;
        d3d12_static_sampler.MaxLOD = D3D12_FLOAT32_MAX;
        d3d12_static_sampler.ShaderRegister = 0;
        d3d12_static_sampler.RegisterSpace = 0;
        d3d12_static_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        auto d3d12_root_signature = D3D12_VERSIONED_ROOT_SIGNATURE_DESC{};
        d3d12_root_signature.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        d3d12_root_signature.Desc_1_1 = {.NumParameters = 1,
                                         .pParameters = &d3d12_root_parameter,
                                         .NumStaticSamplers = 1,
                                         .pStaticSamplers = &d3d12_static_sampler,
                                         .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                  D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
                                                  D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED};

        winrt::com_ptr<ID3DBlob> blob;
        THROW_IF_FAILED(::D3D12SerializeVersionedRootSignature(&d3d12_root_signature, blob.put(), nullptr));
        THROW_IF_FAILED(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
                                                    __uuidof(ID3D12RootSignature), root_signature.put_void()));
    }

    auto PipelineCache::get(
        DX12Shader& shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blend_color,
        std::optional<DepthStencilStageInfo> const depth_stencil,
        std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& render_target_formats,
        DXGI_FORMAT const depth_stencil_format) -> core::ref_ptr<Pipeline>
    {
        auto entry = Entry{shader.get_hash(),     rasterizer,          blend_color, depth_stencil,
                           render_target_formats, depth_stencil_format};

        std::unique_lock lock(mutex);
        auto result = entries.find(entry);
        if (result != entries.end())
        {
            return result->second;
        }
        else
        {
            lock.unlock();
            auto pipeline =
                core::make_ref<Pipeline>(device, root_signature.get(), shader, rasterizer, blend_color, depth_stencil,
                                         render_target_formats, depth_stencil_format, nullptr);

            lock.lock();
            entries.emplace(entry, pipeline);
            return pipeline;
        }
    }

    auto PipelineCache::reset() -> void
    {
        std::lock_guard lock(mutex);
        entries.clear();
    }

    DX12FutureImpl::DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fence_event,
                                   uint64_t const fence_value)
        : queue(queue), fence(fence), fence_event(fence_event), fence_value(fence_value)
    {
    }

    auto DX12FutureImpl::get_result() const -> bool
    {
        return fence->GetCompletedValue() >= fence_value;
    }

    auto DX12FutureImpl::wait() -> void
    {
        if (fence->GetCompletedValue() < fence_value)
        {
            THROW_IF_FAILED(fence->SetEventOnCompletion(fence_value, fence_event));
            ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
        }
    }

    auto render_pass_load_to_d3d12(RenderPassLoadOp const load_op) -> D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE
    {
        switch (load_op)
        {
            case RenderPassLoadOp::Clear:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            case RenderPassLoadOp::Load:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
            case RenderPassLoadOp::DontCare:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
            default:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }
    }

    auto render_pass_store_to_d3d12(RenderPassStoreOp const store_op) -> D3D12_RENDER_PASS_ENDING_ACCESS_TYPE
    {
        switch (store_op)
        {
            case RenderPassStoreOp::Store:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
            case RenderPassStoreOp::DontCare:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
            default:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }
    }

    auto resource_state_to_d3d12(ResourceState const state) -> D3D12_RESOURCE_STATES
    {
        switch (state)
        {
            case ResourceState::Common:
                return D3D12_RESOURCE_STATE_COMMON;
            case ResourceState::DepthRead:
                return D3D12_RESOURCE_STATE_DEPTH_READ;
            case ResourceState::DepthWrite:
                return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            case ResourceState::RenderTarget:
                return D3D12_RESOURCE_STATE_RENDER_TARGET;
            case ResourceState::UnorderedAccess:
                return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            case ResourceState::PixelShaderRead:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            case ResourceState::NonPixelShaderRead:
                return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            case ResourceState::AllShaderRead:
                return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            case ResourceState::CopySrc:
                return D3D12_RESOURCE_STATE_COPY_SOURCE;
            case ResourceState::CopyDst:
                return D3D12_RESOURCE_STATE_COPY_DEST;
            default:
                return D3D12_RESOURCE_STATE_COMMON;
        }
    }

    DX12GraphicsContext::DX12GraphicsContext(ID3D12Device4* device, PipelineCache& pipeline_cache,
                                             DescriptorAllocator& descriptor_allocator, ID3D12CommandQueue* queue,
                                             ID3D12Fence* fence, HANDLE fence_event, uint64_t& fence_value)
        : pipeline_cache(&pipeline_cache), descriptor_allocator(&descriptor_allocator), queue(queue), fence(fence),
          fence_event(fence_event), fence_value(&fence_value)
    {
        THROW_IF_FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator),
                                                       command_allocator.put_void()));

        THROW_IF_FAILED(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
                                                   __uuidof(ID3D12GraphicsCommandList4), command_list.put_void()));
    }

    auto DX12GraphicsContext::reset() -> void
    {
        THROW_IF_FAILED(command_list->Reset(command_allocator.get(), nullptr));

        if (descriptor_allocator)
        {
            auto descriptors = std::array<ID3D12DescriptorHeap*, 2>{
                descriptor_allocator->get_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
                descriptor_allocator->get_heap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)};
            command_list->SetDescriptorHeaps(static_cast<uint32_t>(descriptors.size()), descriptors.data());
        }

        is_root_signature_binded = false;
        current_shader = nullptr;
    }

    auto DX12GraphicsContext::set_graphics_pipeline_options(core::ref_ptr<Shader> shader,
                                                            RasterizerStageInfo const& rasterizer,
                                                            BlendColorInfo const& blend_color,
                                                            std::optional<DepthStencilStageInfo> const depth_stencil)
        -> void
    {
        auto pipeline = pipeline_cache->get(static_cast<DX12Shader&>(*shader), rasterizer, blend_color, depth_stencil,
                                            render_target_formats, depth_stencil_format);

        if (!is_root_signature_binded)
        {
            command_list->SetGraphicsRootSignature(pipeline->get_root_signature());
            is_root_signature_binded = true;
        }

        command_list->SetPipelineState(pipeline->get_pipeline_state());
        command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        current_shader = shader;
    }

    auto DX12GraphicsContext::bind_descriptor(std::string_view const binding, uint32_t const descriptor) -> void
    {
        uint32_t const index = current_shader->get_bindings().at(std::string(binding)).binding;
        bindings[index] = descriptor;
    }

    auto DX12GraphicsContext::begin_render_pass(std::span<RenderPassColorInfo> const colors,
                                                std::optional<RenderPassDepthStencilInfo> depth_stencil) -> void
    {
        render_target_formats.fill(DXGI_FORMAT_UNKNOWN);
        depth_stencil_format = DXGI_FORMAT_UNKNOWN;

        auto render_pass_render_targets =
            std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>{};

        for (uint32_t const i : std::views::iota(0u, colors.size()))
        {
            auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
            begin.Type = render_pass_load_to_d3d12(colors[i].load_op);

            begin.Clear.ClearValue.Format = texture_format_to_dxgi(colors[i].texture->get_format());
            begin.Clear.ClearValue.Color[0] = colors[i].clear_color.r;
            begin.Clear.ClearValue.Color[1] = colors[i].clear_color.g;
            begin.Clear.ClearValue.Color[2] = colors[i].clear_color.b;
            begin.Clear.ClearValue.Color[3] = colors[i].clear_color.a;

            auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
            end.Type = render_pass_store_to_d3d12(colors[i].store_op);

            render_pass_render_targets[i].BeginningAccess = begin;
            render_pass_render_targets[i].EndingAccess = end;
            render_pass_render_targets[i].cpuDescriptor = static_cast<DX12Texture*>(colors[i].texture.get())
                                                              ->get_descriptor(TextureUsage::RenderTarget)
                                                              .cpu_handle();

            render_target_formats[i] = begin.Clear.ClearValue.Format;
        }

        if (depth_stencil.has_value())
        {
            auto value = depth_stencil.value();
            depth_stencil_format = texture_format_to_dxgi(value.texture->get_format());

            auto render_pass_depth_stencil = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC{};
            {
                auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
                begin.Type = render_pass_load_to_d3d12(value.depth_load_op);
                begin.Clear.ClearValue.Format = depth_stencil_format;
                begin.Clear.ClearValue.DepthStencil.Depth = value.clear_depth;

                auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
                end.Type = render_pass_store_to_d3d12(value.depth_store_op);

                render_pass_depth_stencil.DepthBeginningAccess = begin;
                render_pass_depth_stencil.DepthEndingAccess = end;
            }

            {
                auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
                begin.Type = render_pass_load_to_d3d12(value.stencil_load_op);
                begin.Clear.ClearValue.Format = depth_stencil_format;
                begin.Clear.ClearValue.DepthStencil.Depth = value.clear_depth;

                auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
                end.Type = render_pass_store_to_d3d12(value.stencil_store_op);

                render_pass_depth_stencil.StencilBeginningAccess = begin;
                render_pass_depth_stencil.StencilEndingAccess = end;
            }
            render_pass_depth_stencil.cpuDescriptor =
                static_cast<DX12Texture*>(value.texture.get())->get_descriptor(TextureUsage::DepthStencil).cpu_handle();

            command_list->BeginRenderPass(static_cast<uint32_t>(colors.size()), render_pass_render_targets.data(),
                                          &render_pass_depth_stencil, D3D12_RENDER_PASS_FLAG_NONE);
        }
        else
        {
            command_list->BeginRenderPass(static_cast<uint32_t>(colors.size()), render_pass_render_targets.data(),
                                          nullptr, D3D12_RENDER_PASS_FLAG_NONE);
        }
    }

    auto DX12GraphicsContext::end_render_pass() -> void
    {
        command_list->EndRenderPass();
    }

    auto DX12GraphicsContext::bind_vertex_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
        -> void
    {
        auto d3d12_vertex_buffer_view = D3D12_VERTEX_BUFFER_VIEW{};
        d3d12_vertex_buffer_view.BufferLocation =
            static_cast<DX12Buffer*>(buffer.get())->get_resource()->GetGPUVirtualAddress() + offset;
        d3d12_vertex_buffer_view.SizeInBytes = static_cast<uint32_t>(size);
        d3d12_vertex_buffer_view.StrideInBytes = current_shader->get_inputs_size_per_vertex();

        command_list->IASetVertexBuffers(0, 1, &d3d12_vertex_buffer_view);
    }

    auto DX12GraphicsContext::bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                                IndexFormat const format) -> void
    {
        auto d3d12_index_buffer_view = D3D12_INDEX_BUFFER_VIEW{};
        d3d12_index_buffer_view.BufferLocation =
            static_cast<DX12Buffer*>(buffer.get())->get_resource()->GetGPUVirtualAddress() + offset;
        switch (format)
        {
            case IndexFormat::Uint32: {
                d3d12_index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
                break;
            }
            case IndexFormat::Uint16: {
                d3d12_index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
                break;
            }
        }
        d3d12_index_buffer_view.SizeInBytes = size;

        command_list->IASetIndexBuffer(&d3d12_index_buffer_view);
    }

    auto DX12GraphicsContext::draw_indexed(uint32_t const index_count, uint32_t const instance_count) -> void
    {
        command_list->SetGraphicsRoot32BitConstants(0, static_cast<uint32_t>(bindings.size()), bindings.data(), 0);

        command_list->DrawIndexedInstanced(index_count, instance_count, 0, 0, 0);
    }

    auto DX12GraphicsContext::draw(uint32_t const vertex_count, uint32_t const instance_count) -> void
    {
        command_list->SetGraphicsRoot32BitConstants(0, static_cast<uint32_t>(bindings.size()), bindings.data(), 0);

        command_list->DrawInstanced(vertex_count, instance_count, 0, 0);
    }

    auto DX12GraphicsContext::set_viewport(int32_t const x, int32_t const y, uint32_t const width,
                                           uint32_t const height) -> void
    {
        auto d3d12_viewport = D3D12_VIEWPORT{};
        d3d12_viewport.TopLeftX = static_cast<float>(x);
        d3d12_viewport.TopLeftY = static_cast<float>(y);
        d3d12_viewport.Width = static_cast<float>(width);
        d3d12_viewport.Height = static_cast<float>(height);
        d3d12_viewport.MinDepth = D3D12_MIN_DEPTH;
        d3d12_viewport.MaxDepth = D3D12_MAX_DEPTH;

        command_list->RSSetViewports(1, &d3d12_viewport);
    }

    auto DX12GraphicsContext::set_scissor(int32_t const left, int32_t const top, int32_t const right,
                                          int32_t const bottom) -> void
    {
        auto d3d12_rect = D3D12_RECT{};
        d3d12_rect.top = static_cast<LONG>(top);
        d3d12_rect.bottom = static_cast<LONG>(bottom);
        d3d12_rect.left = static_cast<LONG>(left);
        d3d12_rect.right = static_cast<LONG>(right);

        command_list->RSSetScissorRects(1, &d3d12_rect);
    }

    auto DX12GraphicsContext::barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst,
                                      ResourceState const before, ResourceState const after) -> void
    {
        auto d3d12_resource_barrier = D3D12_RESOURCE_BARRIER{};
        d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12_resource_barrier.Transition.StateBefore = resource_state_to_d3d12(before);
        d3d12_resource_barrier.Transition.StateAfter = resource_state_to_d3d12(after);
        d3d12_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        utils::variant_match(dst)
            .case_<core::ref_ptr<Buffer>>([&](auto& data) {
                d3d12_resource_barrier.Transition.pResource = static_cast<DX12Buffer*>(data.get())->get_resource();
            })
            .case_<core::ref_ptr<Texture>>([&](auto& data) {
                d3d12_resource_barrier.Transition.pResource = static_cast<DX12Texture*>(data.get())->get_resource();
            });

        command_list->ResourceBarrier(1, &d3d12_resource_barrier);
    }

    auto DX12GraphicsContext::execute() -> Future<Query>
    {
        THROW_IF_FAILED(command_list->Close());

        auto command_batches =
            std::array<ID3D12CommandList*, 1>{reinterpret_cast<ID3D12CommandList*>(command_list.get())};

        queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());
        (*fence_value)++;
        THROW_IF_FAILED(queue->Signal(fence, *fence_value));

        auto query = core::make_ref<DX12Query>();
        auto future_impl = std::make_unique<DX12FutureImpl>(queue, fence, fence_event, *fence_value);
        return Future<Query>(query, std::move(future_impl));
    }

    auto get_surface_data(rhi::TextureFormat const format, uint32_t const width, uint32_t const height,
                          size_t& row_bytes, uint32_t& row_count) -> void
    {
        bool bc = false;
        uint32_t bpe = 0;

        switch (format)
        {
            case rhi::TextureFormat::BC1:
            case rhi::TextureFormat::BC4: {
                bc = true;
                bpe = 8;
                break;
            }
            case rhi::TextureFormat::BC3:
            case rhi::TextureFormat::BC5: {
                bc = true;
                bpe = 16;
                break;
            }
        }

        if (bc)
        {
            uint64_t const block_wide = std::max<uint64_t>(1u, (static_cast<uint64_t>(width) + 3u) / 4u);
            uint64_t const block_high = std::max<uint64_t>(1u, (static_cast<uint64_t>(height) + 3u) / 4u);
            row_bytes = block_wide * bpe;
            row_count = static_cast<uint32_t>(block_high);
        }
        else
        {
            size_t const bpp = 32;
            row_bytes = (width * bpp + 7) / 8;
            row_count = height;
        }
    }

    DX12CopyContext::DX12CopyContext(ID3D12Device4* device, ID3D12CommandQueue* queue, ID3D12Fence* fence,
                                     HANDLE fence_event, uint64_t& fence_value)
        : device(device), queue(queue), fence(fence), fence_event(fence_event), fence_value(&fence_value)
    {
        THROW_IF_FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, __uuidof(ID3D12CommandAllocator),
                                                       command_allocator.put_void()));

        THROW_IF_FAILED(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE,
                                                   __uuidof(ID3D12GraphicsCommandList4), command_list.put_void()));

        memory_allocator = core::make_ref<MemoryAllocator>(device, 512 * 1024, 16 * 1024 * 1024);

        {
            write_info.buffer = core::make_ref<DX12Buffer>(device, *memory_allocator, nullptr, 16 * 1024 * 1024, 0,
                                                           (BufferUsageFlags)(BufferUsage::MapWrite));
            write_info.offset = 0;
        }

        {
            read_info.buffer = core::make_ref<DX12Buffer>(device, *memory_allocator, nullptr, 16 * 1024 * 1024, 0,
                                                          (BufferUsageFlags)(BufferUsage::MapRead));
            read_info.offset = 0;
        }
    }

    auto DX12CopyContext::reset() -> void
    {
        THROW_IF_FAILED(command_list->Reset(command_allocator.get(), nullptr));
    }

    auto DX12CopyContext::write_buffer(core::ref_ptr<Buffer> dst, std::span<uint8_t const> const data) -> Future<Buffer>
    {
        if (data.size() >= write_info.buffer->get_size() - write_info.offset)
        {
            execute();
            reset();
        }

        uint64_t offset = 0;
        {
            uint8_t* mapped_bytes = write_info.buffer->map_memory();
            std::basic_spanstream<uint8_t> stream(
                std::span<uint8_t>(mapped_bytes + write_info.offset, write_info.buffer->get_size()),
                std::ios::binary | std::ios::out);
            offset = stream.tellg();
            stream.write(data.data(), data.size());
            write_info.offset = stream.tellg();
            write_info.buffer->unmap_memory();
        }

        command_list->CopyBufferRegion(static_cast<DX12Buffer&>(*dst).get_resource(), 0,
                                       static_cast<DX12Buffer&>(*write_info.buffer).get_resource(), offset,
                                       data.size());

        auto future_impl = std::make_unique<DX12FutureImpl>(queue, fence, fence_event, (*fence_value) + 1);
        return Future<Buffer>(dst, std::move(future_impl));
    }

    auto DX12CopyContext::write_texture(core::ref_ptr<Texture> dst, std::vector<std::span<uint8_t const>> const& data)
        -> Future<Texture>
    {
        size_t row_bytes = 0;
        uint32_t row_count = 0;
        size_t total_bytes = 0;

        uint32_t const row_pitch_alignment_mask = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1;
        uint32_t const resource_alignment_mask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;
        uint32_t const stride = 4;

        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints;
        footprints.resize(dst->get_mip_levels());

        auto d3d12_resource_desc = static_cast<DX12Texture&>(*dst).get_resource()->GetDesc();

        device->GetCopyableFootprints(&d3d12_resource_desc, 0, dst->get_mip_levels(), 0, footprints.data(), nullptr,
                                      nullptr, &total_bytes);

        if (total_bytes >= write_info.buffer->get_size() - write_info.offset)
        {
            execute();
            reset();
        }

        {
            uint8_t* mapped_bytes = write_info.buffer->map_memory();
            std::basic_spanstream<uint8_t> stream(
                std::span<uint8_t>(mapped_bytes + write_info.offset, write_info.buffer->get_size()),
                std::ios::binary | std::ios::out);

            for (uint32_t const i : std::views::iota(0u, data.size()))
            {
                get_surface_data(dst->get_format(), footprints[i].Footprint.Width, footprints[i].Footprint.Height,
                                 row_bytes, row_count);

                footprints[i].Footprint.RowPitch =
                    (footprints[i].Footprint.Width * stride * row_pitch_alignment_mask) & ~row_pitch_alignment_mask;
                footprints[i].Offset = stream.tellg();

                for (uint32_t const j : std::views::iota(0u, row_count))
                {
                    stream.write(data[i].data() + row_bytes * j, row_bytes);
                    stream.seekg(footprints[i].Footprint.RowPitch - row_bytes, std::ios::cur);
                }

                size_t const total_size = (uint64_t)stream.tellg() - footprints[i].Offset;
                stream.seekg(((total_size + resource_alignment_mask) & ~resource_alignment_mask) - total_size,
                             std::ios::cur);
            }

            write_info.offset = stream.tellg();
            write_info.buffer->unmap_memory();
        }

        for (uint32_t const i : std::views::iota(0u, data.size()))
        {
            auto d3d12_source_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_source_location.pResource = static_cast<DX12Buffer&>(*write_info.buffer).get_resource();
            d3d12_source_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            d3d12_source_location.PlacedFootprint = footprints[i];

            auto d3d12_dest_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_dest_location.pResource = static_cast<DX12Texture&>(*dst).get_resource();
            d3d12_dest_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            d3d12_dest_location.SubresourceIndex = i;

            command_list->CopyTextureRegion(&d3d12_dest_location, 0, 0, 0, &d3d12_source_location, nullptr);
        }

        auto future_impl = std::make_unique<DX12FutureImpl>(queue, fence, fence_event, (*fence_value) + 1);
        return Future<Texture>(dst, std::move(future_impl));
    }

    auto DX12CopyContext::read_buffer(core::ref_ptr<Buffer> dst, std::vector<uint8_t>& data) -> void
    {
        command_list->CopyBufferRegion(static_cast<DX12Buffer&>(*read_info.buffer).get_resource(), read_info.offset,
                                       static_cast<DX12Buffer&>(*dst).get_resource(), 0, dst->get_size());

        Future<Query> result = execute();
        result.wait();
        reset();

        {
            uint8_t* mapped_bytes = read_info.buffer->map_memory();
            std::basic_spanstream<uint8_t> stream(
                std::span<uint8_t>(mapped_bytes + read_info.offset, read_info.buffer->get_size()),
                std::ios::binary | std::ios::in);
            data.resize(dst->get_size());
            stream.read(data.data(), data.size());
            read_info.offset = stream.tellg();
            read_info.buffer->unmap_memory();
        }
    }

    auto DX12CopyContext::read_texture(core::ref_ptr<Texture> dst, std::vector<std::vector<uint8_t>>& data) -> void
    {
        size_t row_bytes = 0;
        uint32_t row_count = 0;
        size_t total_bytes = 0;

        uint32_t const resource_alignment_mask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;

        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints;
        footprints.resize(dst->get_mip_levels());

        auto d3d12_resource_desc = static_cast<DX12Texture&>(*dst).get_resource()->GetDesc();

        device->GetCopyableFootprints(&d3d12_resource_desc, 0, dst->get_mip_levels(), 0, footprints.data(), nullptr,
                                      nullptr, &total_bytes);

        for (uint32_t const i : std::views::iota(0u, footprints.size()))
        {
            auto d3d12_source_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_source_location.pResource = static_cast<DX12Texture&>(*dst).get_resource();
            d3d12_source_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            d3d12_source_location.SubresourceIndex = i;

            auto d3d12_dest_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_dest_location.pResource = static_cast<DX12Buffer&>(*read_info.buffer).get_resource();
            d3d12_dest_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            d3d12_dest_location.PlacedFootprint = footprints[i];

            command_list->CopyTextureRegion(&d3d12_dest_location, 0, 0, 0, &d3d12_source_location, nullptr);
        }

        Future<Query> result = execute();
        result.wait();
        reset();

        {
            uint8_t* mapped_bytes = read_info.buffer->map_memory();
            std::basic_spanstream<uint8_t> stream(
                std::span<uint8_t>(mapped_bytes + read_info.offset, read_info.buffer->get_size()),
                std::ios::binary | std::ios::in);

            data.resize(footprints.size());

            for (uint32_t const i : std::views::iota(0u, footprints.size()))
            {
                get_surface_data(dst->get_format(), footprints[i].Footprint.Width, footprints[i].Footprint.Height,
                                 row_bytes, row_count);

                data[i].resize(row_bytes * row_count);

                for (uint32_t const j : std::views::iota(0u, row_count))
                {
                    stream.read(data[i].data() + row_bytes * j, row_bytes);
                    stream.seekg(footprints[i].Footprint.RowPitch - row_bytes, std::ios::cur);
                }

                size_t const total_size = (uint64_t)stream.tellg() - footprints[i].Offset;
                stream.seekg(((total_size + resource_alignment_mask) & ~resource_alignment_mask) - total_size,
                             std::ios::cur);
            }

            read_info.offset = stream.tellg();
            read_info.buffer->unmap_memory();
        }
    }

    auto DX12CopyContext::barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst,
                                  ResourceState const before, ResourceState const after) -> void
    {
        auto d3d12_resource_barrier = D3D12_RESOURCE_BARRIER{};
        d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12_resource_barrier.Transition.StateBefore = resource_state_to_d3d12(before);
        d3d12_resource_barrier.Transition.StateAfter = resource_state_to_d3d12(after);
        d3d12_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        utils::variant_match(dst)
            .case_<core::ref_ptr<Buffer>>([&](auto& data) {
                d3d12_resource_barrier.Transition.pResource = static_cast<DX12Buffer*>(data.get())->get_resource();
            })
            .case_<core::ref_ptr<Texture>>([&](auto& data) {
                d3d12_resource_barrier.Transition.pResource = static_cast<DX12Texture*>(data.get())->get_resource();
            });

        command_list->ResourceBarrier(1, &d3d12_resource_barrier);
    }

    auto DX12CopyContext::execute() -> Future<Query>
    {
        write_info.offset = 0;
        read_info.offset = 0;

        THROW_IF_FAILED(command_list->Close());

        auto command_batches =
            std::array<ID3D12CommandList*, 1>{reinterpret_cast<ID3D12CommandList*>(command_list.get())};

        queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());
        (*fence_value)++;
        THROW_IF_FAILED(queue->Signal(fence, *fence_value));

        auto query = core::make_ref<DX12Query>();
        auto future_impl = std::make_unique<DX12FutureImpl>(queue, fence, fence_event, *fence_value);
        return Future<Query>(query, std::move(future_impl));
    }

    DX12Device::DX12Device(platform::Window* window)
    {
#ifdef _DEBUG
        THROW_IF_FAILED(::D3D12GetDebugInterface(__uuidof(ID3D12Debug1), debug.put_void()));
        debug->EnableDebugLayer();
        debug->SetEnableGPUBasedValidation(true);
        debug->SetEnableSynchronizedCommandQueueValidation(true);

        uint32_t factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#else
        uint32_t factory_flags = 0;
#endif

        THROW_IF_FAILED(::CreateDXGIFactory2(factory_flags, __uuidof(IDXGIFactory4), factory.put_void()));

        {
            winrt::com_ptr<IDXGIAdapter1> adapter;

            for (uint32_t i = 0; factory->EnumAdapters1(i, adapter.put()) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                auto dxgi_adapter_desc = DXGI_ADAPTER_DESC1{};
                THROW_IF_FAILED(adapter->GetDesc1(&dxgi_adapter_desc));

                if (dxgi_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }
                else
                {
                    this->adapter = adapter;
                    break;
                }
            }
        }

        THROW_IF_FAILED(
            ::D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), device.put_void()));

        fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!fence_event)
        {
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        }

        {
            {
                auto d3d12_queue_desc = D3D12_COMMAND_QUEUE_DESC{};
                d3d12_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                d3d12_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

                THROW_IF_FAILED(device->CreateCommandQueue(&d3d12_queue_desc, __uuidof(ID3D12CommandQueue),
                                                           graphics_info.queue.put_void()));
            }

            THROW_IF_FAILED(
                device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), graphics_info.fence.put_void()));

            graphics_info.fence_value = 0;
        }

        {
            {
                auto d3d12_queue_desc = D3D12_COMMAND_QUEUE_DESC{};
                d3d12_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                d3d12_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

                THROW_IF_FAILED(device->CreateCommandQueue(&d3d12_queue_desc, __uuidof(ID3D12CommandQueue),
                                                           copy_info.queue.put_void()));
            }

            THROW_IF_FAILED(
                device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), copy_info.fence.put_void()));

            copy_info.fence_value = 0;
        }

        {
            {
                auto d3d12_queue_desc = D3D12_COMMAND_QUEUE_DESC{};
                d3d12_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                d3d12_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

                THROW_IF_FAILED(device->CreateCommandQueue(&d3d12_queue_desc, __uuidof(ID3D12CommandQueue),
                                                           compute_info.queue.put_void()));
            }

            THROW_IF_FAILED(
                device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), compute_info.fence.put_void()));

            compute_info.fence_value = 0;
        }

        descriptor_allocator = core::make_ref<DescriptorAllocator>(device.get());
        memory_allocator = core::make_ref<MemoryAllocator>(device.get(), 1024 * 1024, 64 * 1024 * 1024);
        pipeline_cache = core::make_ref<PipelineCache>(device.get());

        if (window)
        {
            {
                auto dxgi_swapchain_desc = DXGI_SWAP_CHAIN_DESC1{};
                dxgi_swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                dxgi_swapchain_desc.Width = window->get_width();
                dxgi_swapchain_desc.Height = window->get_height();
                dxgi_swapchain_desc.BufferCount = 2;
                dxgi_swapchain_desc.SampleDesc.Count = 1;
                dxgi_swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                dxgi_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                dxgi_swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

                winrt::com_ptr<IDXGISwapChain1> swapchain;
                THROW_IF_FAILED(factory->CreateSwapChainForHwnd(
                    graphics_info.queue.get(), reinterpret_cast<HWND>(window->get_native_handle()),
                    &dxgi_swapchain_desc, nullptr, nullptr, swapchain.put()));
                swapchain.as(this->swapchain);
            }

            for (uint32_t const i : std::views::iota(0u, 2u))
            {
                core::ref_ptr<Texture> texture;
                {
                    winrt::com_ptr<ID3D12Resource> resource;
                    THROW_IF_FAILED(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));

                    texture = core::make_ref<DX12Texture>(device.get(), resource, *descriptor_allocator,
                                                          (TextureUsageFlags)TextureUsage::RenderTarget);
                }
                back_buffers.emplace_back(texture);
            }
        }
    }

    DX12Device::~DX12Device()
    {
        ::CloseHandle(fence_event);
    }

    auto DX12Device::create_shader(std::span<uint8_t const> const data) -> core::ref_ptr<Shader>
    {
        std::lock_guard lock(mutex);

        auto shader = core::make_ref<DX12Shader>(device.get(), data);

        return shader;
    }

    auto DX12Device::create_texture(uint32_t const width, uint32_t const height, uint32_t const depth,
                                    uint32_t const mip_levels, TextureFormat const format,
                                    TextureDimension const dimension, TextureUsageFlags const flags)
        -> core::ref_ptr<Texture>
    {
        std::lock_guard lock(mutex);

        auto texture = core::make_ref<DX12Texture>(device.get(), *memory_allocator, *descriptor_allocator, width,
                                                   height, depth, mip_levels, format, dimension, flags);

        return texture;
    }

    auto DX12Device::create_buffer(size_t const size, size_t const element_stride, BufferUsageFlags const flags)
        -> core::ref_ptr<Buffer>
    {
        std::lock_guard lock(mutex);

        auto buffer = core::make_ref<DX12Buffer>(device.get(), *memory_allocator, descriptor_allocator.get(), size,
                                                 element_stride, flags);

        return buffer;
    }

    auto DX12Device::create_graphics_context() -> core::ref_ptr<GraphicsContext>
    {
        std::lock_guard lock(mutex);

        auto context = core::make_ref<DX12GraphicsContext>(device.get(), *pipeline_cache, *descriptor_allocator,
                                                           graphics_info.queue.get(), graphics_info.fence.get(),
                                                           fence_event, graphics_info.fence_value);

        return context;
    }

    auto DX12Device::create_copy_context() -> core::ref_ptr<CopyContext>
    {
        std::lock_guard lock(mutex);

        auto context = core::make_ref<DX12CopyContext>(device.get(), copy_info.queue.get(), copy_info.fence.get(),
                                                       fence_event, copy_info.fence_value);

        return context;
    }

    auto DX12Device::request_back_buffer() -> core::ref_ptr<Texture>
    {
        if (!swapchain)
        {
            throw core::Exception("Swapchain is not found");
        }

        if (graphics_info.fence->GetCompletedValue() < graphics_info.fence_value)
        {
            THROW_IF_FAILED(graphics_info.fence->SetEventOnCompletion(graphics_info.fence_value, fence_event));
            ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
        }

        return back_buffers[swapchain->GetCurrentBackBufferIndex()];
    }

    auto DX12Device::present_back_buffer() -> void
    {
        if (!swapchain)
        {
            throw core::Exception("Swapchain is not found");
        }

        THROW_IF_FAILED(swapchain->Present(0, 0));

        graphics_info.fence_value++;
        THROW_IF_FAILED(graphics_info.queue->Signal(graphics_info.fence.get(), graphics_info.fence_value));
    }
} // namespace ionengine::rhi