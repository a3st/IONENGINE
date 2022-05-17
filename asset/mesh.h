// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_ptr.h>
#include <asset/material.h>
#include <lib/hash/buffer.h>
#include <lib/expected.h>
#include <tinyobjloader/tiny_obj_loader.h>

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

struct SurfaceData {
    lib::hash::Buffer<float> vertices;
    lib::hash::Buffer<uint32_t> indices;
    uint32_t material_index;
    size_t cache_entry{std::numeric_limits<size_t>::max()};
};

class Mesh {
public:

    static lib::Expected<Mesh, lib::Result<MeshError>> load_from_file(std::filesystem::path const& file_path);

    std::span<SurfaceData> surfaces();

    void material(uint32_t const index, AssetPtr<Material> material);

    AssetPtr<Material> material(uint32_t const index);

    uint32_t materials_size() const;

private:

    Mesh(tinyobj::attrib_t const& attributes, std::span<tinyobj::shape_t const> const shapes, std::span<tinyobj::material_t const> const materials);

    std::vector<MeshAttribute> _attributes;
    std::vector<SurfaceData> _surfaces;
    std::vector<AssetPtr<Material>> _materials;
};

}
