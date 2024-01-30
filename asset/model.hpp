// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "glb_model.hpp"
#include "renderer/renderer.hpp"

namespace ionengine {

enum class ModelFormat {
    GLB
};

struct MeshData {
    std::vector<core::ref_ptr<renderer::Primitive>> primitives;
    std::vector<uint32_t> materials;
};

class Model {
public:

    Model(renderer::Renderer& renderer, std::span<uint8_t> const data_bytes, ModelFormat const format, bool const immediate = false);

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