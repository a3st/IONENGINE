// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>

namespace ionengine::renderer {

class MeshSurface {
public:

    MeshSurface() = default;

    bool load_from_data(Backend& backend, MeshSurfaceData const& mesh_surface_data);

private:
    
    Handle<Buffer> _vertices_buffer;
    Handle<Buffer> _indices_buffer;
};

}