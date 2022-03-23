// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>

namespace ionengine::renderer {

struct MeshSurface {

    Handle<Buffer> vertex_positions_buffer;
    Handle<Buffer> vertex_uv_normals_buffer;
    Handle<Buffer> index_buffer;
};

}