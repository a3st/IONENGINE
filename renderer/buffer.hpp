// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>
#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

class Context;

class Buffer : public core::ref_counted_object {
public:

    Buffer(
        Context& context,
        size_t const size,
        wgpu::BufferUsageFlags const usage
    );

    auto upload(std::span<uint8_t const> const data_bytes, uint64_t const offset = 0) -> void;

    auto get_buffer() const -> wgpu::Buffer {

        return buffer;
    }

private:

    Context* context;
    wgpu::Buffer buffer{nullptr};
};

struct BufferAllocation {
    core::ref_ptr<Buffer> buffer{nullptr};
    uint64_t offset;
    size_t size;
};

template<typename Type>
class BufferAllocator {
public:

    BufferAllocator(Context& context, size_t const size, wgpu::BufferUsageFlags const usage) : allocator(context, size, usage) {

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

    LinearAllocator(Context& context, size_t const size, wgpu::BufferUsageFlags const usage);

    auto allocate(size_t const size) -> BufferAllocation;

    auto deallocate(BufferAllocation const& allocation) -> void;

    auto reset() -> void;
};

struct BlockAllocator {

    BlockAllocator(Context& context, size_t const size, wgpu::BufferUsageFlags const usage);
    
    auto allocate(size_t const size) -> BufferAllocation;

    auto deallocate(BufferAllocation const& allocation) -> void;

    auto reset() -> void;
};

}

}