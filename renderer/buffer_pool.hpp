// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"

namespace ionengine {

namespace renderer {

inline uint32_t BUFFER_POOL_CHUNK_DEFAULT_SIZE = 10;

class BufferPool : public core::ref_counted_object {
public:

    using Entry = std::pair<size_t, rhi::BufferUsageFlags>;

    struct EntryHasher {
        auto operator()(const Entry& entry) const -> std::size_t {
            return std::hash<size_t>()(entry.first) ^ std::hash<uint32_t>()(entry.second);
        }
    };

    BufferPool(rhi::Device& device);
    
    auto allocate(size_t const size, rhi::BufferUsageFlags const flags, std::span<uint8_t const> const data) -> core::ref_ptr<rhi::Buffer>;

    auto reset() -> void;

private:

    rhi::Device* device;
    std::unordered_map<rhi::BufferUsageFlags, core::ref_ptr<rhi::MemoryAllocator>> allocators;
    struct Chunk {
        std::vector<core::ref_ptr<rhi::Buffer>> buffers;
        uint32_t offset;
        uint32_t size;
    };
    std::unordered_map<Entry, std::vector<Chunk>, EntryHasher> entries;

    auto create_chunk(size_t const size, rhi::BufferUsageFlags const flags) -> void;
};

}

}