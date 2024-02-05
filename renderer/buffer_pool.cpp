// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "buffer_pool.hpp"
#include "precompiled.h"

namespace ionengine::renderer
{
    BufferPool::BufferPool(rhi::Device& device) : device(&device)
    {
        allocators.emplace(rhi::MemoryAllocatorUsage::Upload,
                           device.create_allocator(32 * 1024, 32 * 1024 * 1024, rhi::MemoryAllocatorUsage::Upload));

        allocators.emplace(rhi::MemoryAllocatorUsage::Default,
                           device.create_allocator(32 * 1024, 32 * 1024 * 1024, rhi::MemoryAllocatorUsage::Default));
    }

    auto BufferPool::allocate(size_t const size, rhi::BufferUsageFlags const flags, std::span<uint8_t const> const data)
        -> core::ref_ptr<rhi::Buffer>
    {
        core::ref_ptr<rhi::Buffer> buffer = nullptr;

        for (auto& chunk : entries[std::make_tuple(size, flags)])
        {
            if (1 > chunk.size - chunk.offset)
            {
                continue;
            }

            buffer = chunk.buffers[chunk.offset];
            chunk.offset++;
            break;
        }

        if (!buffer)
        {
            create_chunk(size, flags);

            auto& chunk = entries[std::make_tuple(size, flags)].back();
            buffer = chunk.buffers[chunk.offset];
            chunk.offset++;
        }

        buffer = device->write_buffer(buffer, data).get();
        return buffer;
    }

    auto BufferPool::reset() -> void
    {
        for (auto& [entry, chunks] : entries)
        {
            for (auto& chunk : chunks)
            {
                if (chunk.offset == 0)
                {
                    continue;
                }

                chunk.offset = 0;
            }
        }
    }

    auto BufferPool::create_chunk(size_t const size, rhi::BufferUsageFlags const flags) -> void
    {
        std::vector<core::ref_ptr<rhi::Buffer>> buffers;
        if (flags & rhi::BufferUsage::MapWrite)
        {
            for (uint32_t i = 0; i < BUFFER_POOL_CHUNK_DEFAULT_SIZE; ++i)
            {
                auto future = device->create_buffer(allocators.at(rhi::MemoryAllocatorUsage::Upload),
                                                    size, flags, {});
                buffers.emplace_back(future.get());
            }
        }
        else
        {
            for (uint32_t i = 0; i < BUFFER_POOL_CHUNK_DEFAULT_SIZE; ++i)
            {
                auto future = device->create_buffer(allocators.at(rhi::MemoryAllocatorUsage::Default), size, flags, {});
                buffers.emplace_back(future.get());
            }
        }

        auto chunk = Chunk{.buffers = std::move(buffers), .offset = 0, .size = BUFFER_POOL_CHUNK_DEFAULT_SIZE};

        entries.try_emplace(std::make_pair(size, flags));
        entries[std::make_pair(size, flags)].emplace_back(std::move(chunk));
    }
} // namespace ionengine::renderer