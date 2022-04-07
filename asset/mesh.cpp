// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/mesh.h>

using namespace ionengine::asset;

Mesh::Mesh(VertexBuffer const& vertex_buffer, IndexBuffer const& index_buffer) 
    : _vertex_buffer(vertex_buffer), _index_buffer(index_buffer) {

}

std::unique_ptr<Mesh> Mesh::make_test_cube() {

    auto attributes = std::vector<VertexAttributeDesc> {

        { 
            .usage = VertexAttributeUsage::Position,
            .data_type = VertexAttributeType::Float32,
            .size = 3,
            .index = 0
        },

        { 
            .usage = VertexAttributeUsage::TexCoord0,
            .data_type = VertexAttributeType::Float32,
            .size = 2,
            .index = 1
        },

        { 
            .usage = VertexAttributeUsage::Normal,
            .data_type = VertexAttributeType::Float32,
            .size = 3,
            .index = 2
        },

        { 
            .usage = VertexAttributeUsage::Tangent,
            .data_type = VertexAttributeType::Float32,
            .size = 3,
            .index = 3
        }
    };

    auto vertices = std::vector<float> {
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,       0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,     0.0f, 1.0f,       0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,     0.0f, 1.0f,       0.0f, 0.0f, 0.0f
    };

    VertexBuffer vertex_buffer(static_cast<uint32_t>(vertices.size()), attributes, 
        std::span<uint8_t const>(reinterpret_cast<uint8_t*>(vertices.data()), vertices.size() * sizeof(float)));

    auto indices = std::vector<uint32_t> {

    };

    IndexBuffer index_buffer(static_cast<uint32_t>(indices.size()), 
        std::span<uint8_t const>(reinterpret_cast<uint8_t*>(indices.data()), indices.size() * sizeof(uint32_t)));

    return std::make_unique<Mesh>(vertex_buffer, index_buffer);
}