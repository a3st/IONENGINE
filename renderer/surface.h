// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>

namespace ionengine::renderer {

struct SurfaceData {
    uint32_t vertex_count;
    uint32_t index_count;
    Handle<backend::Buffer> vertex_buffer;
    Handle<backend::Buffer> index_buffer;
};

class SurfaceCache {
public:

    SurfaceCache() = default;

    

private:

};

}