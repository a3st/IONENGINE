// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "primitive.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Primitive::Primitive(
    rhi::Device& device,
    core::ref_ptr<rhi::MemoryAllocator> allocator,
    uint32_t const index_count,
    std::span<uint8_t const> const vertices,
    std::span<uint8_t const> const indices,
    bool const immediate
) : 
    index_count(index_count),
    vertices(
        device.create_buffer(
            allocator, 
            vertices.size(), 
            (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex),
            vertices
        )
    ),
    indices(
        device.create_buffer(
            allocator,
            indices.size(),
            (rhi::BufferUsageFlags)(rhi::BufferUsage::Index),
            indices
        )
    )
{
    if(immediate) {
        this->vertices.wait();
        this->indices.wait();
    }
}