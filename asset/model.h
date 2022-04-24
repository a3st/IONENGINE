// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>
#include <lib/hash/buffer_view.h>

namespace ionengine::asset {

enum class VertexUsage : uint8_t {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    Color0 = 3,
    TexCoord0 = 4,
    TexCoord1 = 5,
    TexCoord2 = 6
};

enum class VertexFormat : uint8_t {
    F32x4x4,
    F32x4,
    F32x3,
    F32x2,
    F32
};

struct PrimitiveAttribute {
    VertexUsage usage;
    VertexFormat format;
    uint32_t index;
};

struct Primitive {
    lib::hash::BufferView vertex_buffer;
    std::optional<lib::hash::BufferView> index_buffer;
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t material_index;
};

struct Surface {
    std::vector<PrimitiveAttribute> attributes;
    std::vector<Primitive> primitives;
};

class Model : public Asset {
public:

    Model(std::filesystem::path const& file_path);

    std::span<Surface const> surfaces() const;

private:

    std::vector<Surface> _surfaces;
    std::vector<uint8_t> _data;
};

}
