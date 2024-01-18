// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "glb_model.hpp"
#include "renderer/primitive.hpp"

namespace ionengine {

enum class ModelFormat {
    GLB
};

struct PrimitiveData {
    std::vector<uint8_t> buffer;
    std::span<uint8_t const> vertices;
    std::span<uint8_t const> indices;
};

struct MeshData {
    std::vector<PrimitiveData> primitives;
    std::vector<uint32_t> materials;
};

class Model {
public:

    Model(std::span<uint8_t> const data_bytes, ModelFormat const format);

    auto get_size() const -> size_t {

        return meshes.size();
    }

    auto get_mesh(uint32_t const index) const -> MeshData const& {

        return meshes[index];
    }

private:

    std::vector<MeshData> meshes;
};

}