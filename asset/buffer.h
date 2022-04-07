// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::asset {

enum class VertexAttributeUsage : uint8_t {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    TexCoord0 = 3,
    TexCoord1 = 4,
    TexCoord2 = 5
};

enum class VertexAttributeType : uint8_t {
    Float32,
    UInt32,
    UInt16,
    UInt8
};

struct VertexAttributeDesc {
    VertexAttributeUsage usage;
    VertexAttributeType data_type;
    uint8_t size;
    uint8_t index;
};

class VertexBuffer {
public:

    VertexBuffer(uint32_t const vertex_count, std::span<VertexAttributeDesc const> const attributes, std::span<uint8_t const> const data);

    VertexBuffer(VertexBuffer const& other);

    uint32_t vertex_count() const;

    uint8_t vertex_size() const;

    uint64_t data_hash() const;

    std::span<uint8_t const> data() const;

private:

    std::vector<VertexAttributeDesc> _dense_attributes;
    std::array<std::optional<VertexAttributeDesc>, 6> _sparse_attributes;
    uint8_t _vertex_size{0};
    uint32_t _vertex_count{0};
    std::vector<uint8_t> _data;
    uint64_t _data_hash{0};
};

struct IndexBuffer {
public:

    IndexBuffer(uint32_t const index_count, std::span<uint8_t const> const data);

    uint32_t index_count() const;

    uint64_t data_hash() const;

    std::span<uint8_t const> data() const;

private:

    uint32_t _index_count;
    std::vector<uint8_t> _data;
    uint64_t _data_hash;
};

}
