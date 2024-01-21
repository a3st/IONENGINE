// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "allocator.hpp"
#include "future.hpp"

namespace ionengine {

namespace platform {
class Window;
}

namespace renderer {

namespace rhi {

enum class BackendType {
    DirectX12,
    Vulkan
};

class Device : public core::ref_counted_object {
public:

    static auto create(BackendType const backend_type, platform::Window const& window) -> core::ref_ptr<Device>;

    virtual auto create_allocator(size_t const block_size, size_t const chunk_size, BufferUsageFlags const flags) -> core::ref_ptr<MemoryAllocator> = 0;

    virtual auto create_texture() -> Future<Texture> = 0;

    virtual auto create_buffer(
        MemoryAllocator& allocator, 
        size_t const size, 
        BufferUsageFlags const flags, 
        std::span<uint8_t const> const data
    ) -> Future<Buffer> = 0;

    virtual auto write_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, std::span<uint8_t const> const data) -> Future<Buffer> = 0;
};

}

}

}