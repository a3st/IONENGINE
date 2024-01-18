
#pragma once

#include "core/ref_ptr.hpp"
#include "primitive.hpp"
#include "shader.hpp"

namespace ionengine {

namespace renderer {

struct RenderTask {
    core::ref_ptr<Primitive> primitive;
    core::ref_ptr<Shader> shader;
};

}

}