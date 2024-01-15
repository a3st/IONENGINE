// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>
#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

class Backend;

class Buffer : public core::ref_counted_object {
public:

    Buffer(
        Backend& backend,
        size_t const size,
        wgpu::BufferUsageFlags const usage
    );

    auto get_buffer() const -> wgpu::Buffer {

        return buffer;
    }

private:

    Backend* backend;
    wgpu::Buffer buffer{nullptr};
};

struct BufferAllocation {
    core::ref_ptr<Buffer> buffer;
    uint64_t offset;
};

template<typename Type>
class BufferAllocator {
public:

    BufferAllocator(Backend& backend, size_t const size, wgpu::BufferUsageFlags const usage) : allocator(backend, size, usage) {

    }

    auto allocate(size_t const size) -> BufferAllocation {

        return allocator.allocate(size);
    }

    auto deallocate(BufferAllocation const& allocation) -> void {

        allocator.deallocate(allocation);
    }

    auto reset() -> void {

        allocator.reset();
    }

private:

    Type allocator;
};

struct LinearAllocator {

    struct Chunk {
        core::ref_ptr<Buffer> buffer;
        uint64_t offset;
        size_t size;
    };
    
    std::vector<Chunk> chunks;

    inline static size_t const MAX_CHUNK_SIZE = 64 * 1024  * 1024;

    LinearAllocator(Backend& backend, size_t const size, wgpu::BufferUsageFlags const usage);

    auto allocate(size_t const size) -> BufferAllocation;

    auto deallocate(BufferAllocation const& allocation) -> void;

    auto reset() -> void;
};

struct BlockAllocator {

    BlockAllocator(Backend& backend, size_t const size, wgpu::BufferUsageFlags const usage);
    
    auto allocate(size_t const size) -> BufferAllocation;

    auto deallocate(BufferAllocation const& allocation) -> void;

    auto reset() -> void;
};

}

}