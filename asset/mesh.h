// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>

namespace ionengine::asset {

enum class VertexAttributeUsage : uint16_t {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    TexCoord0 = 3,
    TexCoord1 = 4,
    TexCoord2 = 5
};

struct VertexAttributeDesc {
    VertexAttributeUsage usage;
    uint16_t slot;
    uint32_t offset;
};

class VertexBuffer {
public:

    VertexBuffer() = default;

private:

    std::vector<VertexAttributeDesc> _attributes;
    uint32_t _vertex_size;
    uint32_t _vertex_count;
    std::vector<char8_t> _data;
    uint64_t _data_hash;
};

struct TriangleBuffer {
public:

    TriangleBuffer() = default;

private:

    uint32_t _triangle_size;
    uint32_t _triangle_count;
    std::vector<char8_t> _data;
    uint64_t _data_hash;
};

class Mesh : public Asset {
public:

    Mesh() = default;

private:

    VertexBuffer vertex_buffer;
    TriangleBuffer triangle_buffer;
};

}