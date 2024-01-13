// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/mesh.hpp"
#include "renderer/shader.hpp"
#include "renderer/render_queue.hpp"

namespace ionengine {

namespace renderer {

struct MeshRenderOptions {
    core::ref_ptr<Mesh> _m0;
    std::string shader_name;

};

class MeshRenderer {
public:

    MeshRenderer() = default;

    auto add_opaque_draw() -> void;

    auto add_transparent_draw() -> void;

    auto get_opaque_queue() -> RenderQueue&;
};

}

}