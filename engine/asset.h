// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/data.h>

namespace ionengine {

struct MeshFile {
    uint32_t magic;
    uint32_t positions_count;
    uint64_t positions_offset;
    uint32_t uv_normals_count;
    uint64_t uv_normals_offset;
    uint32_t indices_count;
    uint64_t indices_offset;
    renderer::MeshData data;
};

struct TextureFile {
    uint32_t magic;
    
};

struct Asset {
    std::variant<renderer::MeshData, renderer::TextureData> data;
};

}