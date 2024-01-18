// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"

namespace ionengine {

namespace renderer {

class Backend;

struct PrimitiveData {
    std::span<uint8_t const> vertices;
    std::span<uint8_t const> indices;
};

class Primitive : public core::ref_counted_object {
public:

    inline static std::vector<wgpu::VertexAttribute> attributes = {
        WGPUVertexAttribute {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = 0,
            .shaderLocation = 0
        },
        WGPUVertexAttribute {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = 12,
            .shaderLocation = 1
        },
        WGPUVertexAttribute {
            .format = wgpu::VertexFormat::Float32x2,
            .offset = 24,
            .shaderLocation = 2
        },
    };

    Primitive(Backend& backend, BufferAllocator<LinearAllocator>& allocator);

private:

    Backend* backend;
    BufferAllocator<LinearAllocator>* allocator;
    BufferAllocation vertex_buffer;
    BufferAllocation index_buffer;
};

}

}