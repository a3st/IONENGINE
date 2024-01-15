// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "buffer.hpp"
#include "backend.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Buffer::Buffer(
    Backend& backend,
    size_t const size,
    wgpu::BufferUsageFlags const usage
) :
    backend(&backend)
{
    {
        auto descriptor = wgpu::BufferDescriptor{};
        descriptor.size = size;
        descriptor.usage = usage;

        buffer = backend.get_device().createBuffer(descriptor);
    }
}

LinearAllocator::LinearAllocator(Backend& backend, size_t const size, wgpu::BufferUsageFlags const usage) {

    size_t remaining_size = size;
    do {
        size_t const alloc_size = remaining_size > MAX_CHUNK_SIZE ? MAX_CHUNK_SIZE : remaining_size;

        auto chunk = Chunk {
            .buffer = core::make_ref<Buffer>(backend, alloc_size, usage),
            .offset = 0,
            .size = alloc_size
        };

        chunks.emplace_back(chunk);

        std::cout << std::format("allocated {} of {}", alloc_size / 1024, remaining_size / 1024) << std::endl;

        remaining_size -= alloc_size;
    }
    while(remaining_size > 0);
}

auto LinearAllocator::allocate(size_t const size) -> BufferAllocation {

    assert(size < MAX_CHUNK_SIZE && "size allocation cannot be bigger than MAX_CHUNK_SIZE");
    
    auto allocation = BufferAllocation {
        .buffer = nullptr,
        .offset = 0
    };

    for(auto& chunk : chunks) {
        if(size < chunk.size - chunk.offset) {
            continue;
        }

        allocation.buffer = chunk.buffer;
        allocation.offset = chunk.offset;
        chunk.offset += size;
    }
    return allocation;
}

auto LinearAllocator::deallocate(BufferAllocation const& allocation) -> void {

    assert(false && "deallocate is not supported by linear allocation");
}

auto LinearAllocator::reset() -> void {

    for(auto& chunk : chunks) {
        chunk.offset = 0;
    }
}

BlockAllocator::BlockAllocator(Backend& backend, size_t const size, wgpu::BufferUsageFlags const usage) {

}

auto BlockAllocator::allocate(size_t const size) -> BufferAllocation {

    return { nullptr, 0 };
}

auto BlockAllocator::deallocate(BufferAllocation const& allocation) -> void {

}

auto BlockAllocator::reset() -> void {
    
}