// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_ptr.h>
#include <asset/material.h>
#include <lib/hash/buffer.h>
#include <lib/expected.h>

namespace ionengine::asset {

enum class VertexUsage : uint16_t {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    Color = 3,
    TexCoord0 = 4,
    TexCoord1 = 5,
    TexCoord2 = 6,
    BoneIndices = 7,
    BoneWeights = 8
};

enum class VertexFormat : uint16_t {
    F32x4x4,
    F32x4,
    F32x3,
    F32x2,
    F32
};

enum class MeshError {
    IO,
    ParseError
};

struct MeshAttribute {
    VertexUsage usage;
    VertexFormat format;
    uint32_t index;
};

struct Surface {
    lib::hash::Buffer<float> vertices;
    lib::hash::Buffer<uint32_t> indices;
    uint32_t material_index;
    size_t cache_entry{std::numeric_limits<size_t>::max()};
};

class Mesh {
public:

    static lib::Expected<Mesh, lib::Result<MeshError>> load_from_file(std::filesystem::path const& file_path);

    std::span<Surface const> surfaces() const;

    std::span<Surface> surfaces();

private:

    Mesh();

    std::vector<MeshAttribute> _attributes;
    std::vector<Surface> _surfaces;
    std::vector<AssetPtr<Material>> _materials;
};

}
