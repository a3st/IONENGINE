// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/buffer.hpp"
#include "descriptor_allocator.hpp"
#include "allocator.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

class DX12Buffer : public Buffer {
public:

    DX12Buffer();

    DX12Buffer(
        ID3D12Device1* device, 
        MemoryAllocator& memory_allocator,
        DescriptorAllocator& descriptor_allocator,
        size_t const size,
        BufferUsageFlags const flags
    );

    auto get_size() -> size_t override {

        return size;
    }

    auto get_flags() -> BufferUsageFlags override {

        return flags;
    }

protected:

    DX12MemoryAllocator* memory_allocator;
    DescriptorAllocator* descriptor_allocator;
    winrt::com_ptr<ID3D12Resource> resource;
    MemoryAllocation memory_allocation;
    DescriptorAllocation descriptor_allocation;
    size_t size;
    BufferUsageFlags flags;
};

}

}

}