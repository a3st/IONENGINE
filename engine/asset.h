// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/texture_data.h>
#include <renderer/mesh_data.h>

namespace ionengine {

struct MeshFile {
    uint32_t magic;
    uint32_t positions_count;
    uint32_t positions_offset;
    uint32_t uv_normals_count;
    uint32_t uv_normals_offset;
    uint32_t indices_count;
    uint32_t indices_offset;
    renderer::MeshData data;
};

struct Asset {
    std::variant<renderer::MeshData, renderer::TextureData> data;
};

}