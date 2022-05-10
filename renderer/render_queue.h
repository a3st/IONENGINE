// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/mesh.h>
#include <asset/material.h>
#include <lib/math/matrix.h>

namespace ionengine::renderer {

struct SurfaceInstance {
    lib::math::Matrixf model;
};

struct RenderBatch {
    asset::Surface* surface;
    std::vector<SurfaceInstance> instances;
    asset::Material* material;
    uint64_t sort_index;
};

class RenderQueue {
public:

    RenderQueue();

private:

    std::vector<std::optional<RenderBatch>> _batches;
    std::vector<size_t> _sorted;

};

}