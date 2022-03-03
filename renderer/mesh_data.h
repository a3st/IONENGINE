// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

struct MeshData {
    std::vector<float> positions;
    std::vector<float> uv_normals;
    std::vector<uint32_t> indices;

    inline static std::vector<VertexInputDesc> vertex_declaration = {
        VertexInputDesc { "POSITION", 0, Format::RGB32, 0, sizeof(float) * 3 },
        VertexInputDesc { "TEXCOORD", 1, Format::RG32, 1, sizeof(float) * 2 },
        VertexInputDesc { "NORMAL", 2, Format::RGB32, 1, sizeof(float) * 3 }
    };
};

}