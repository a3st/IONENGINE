
#pragma once

#include "core/ref_ptr.hpp"
#include "cache/primitive_cache.hpp"

namespace ionengine {

namespace renderer {

struct RenderTaskData {
    PrimitiveData primitive;
    uint32_t index_count;
};

struct RenderTask {
    Primitive drawable;
    uint32_t index_count;
};

}

}