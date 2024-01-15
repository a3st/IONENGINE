// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "drawable.hpp"

namespace ionengine {

namespace renderer {

class StaticMesh : public Drawable {
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

    StaticMesh(Backend& backend, BufferAllocator<LinearAllocator>& mesh_allocator) : Drawable(backend, mesh_allocator) { 

    }

private:
};

struct MeshData {

};

}

}