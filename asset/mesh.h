// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>
#include <asset/material.h>
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

struct MeshAttribute {
    VertexUsage usage;
    VertexFormat format;
    uint32_t index;
};

struct MeshSurface {
    lib::hash::BufferView<float> vertices;
    lib::hash::BufferView<uint32_t> indices;
    uint32_t material_index;
};

class Mesh : public Asset {
public:

    Mesh(std::filesystem::path const& file_path, AssetManager& asset_manager);

    MeshSurface const& surface(uint32_t const index) const;

private:

    std::vector<float> _mesh_vertices;
    std::vector<uint32_t> _mesh_indices;
    std::vector<MeshAttribute> _mesh_attributes;
    std::vector<MeshSurface> _mesh_surfaces;
    std::vector<std::shared_ptr<Material>> _materials;
};

}
