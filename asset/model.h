// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>

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

class HashedBuffer {
public:

    HashedBuffer(std::span<uint8_t const> const data, size_t const element_size);

    uint64_t hash() const;

    uint8_t const* data() const;

    size_t size() const;

    uint32_t count() const;

private:
    
    std::span<uint8_t const> _data;
    uint64_t _data_hash{0};
    uint32_t _count{0};
};

struct Primitive {
    HashedBuffer vertex_buffer;
    std::optional<HashedBuffer> index_buffer;
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
