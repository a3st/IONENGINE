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
    std::shared_ptr<asset::Surface> surface;
    std::vector<SurfaceInstance> instances;
    asset::AssetPtr<asset::Material> material;
    uint64_t sort_index;
};

class RenderQueue {
public:

    using ConstIterator = std::vector<RenderBatch>::const_iterator;

    using Iterator = std::vector<RenderBatch>::iterator;

    RenderQueue();

    void push(std::shared_ptr<asset::Surface> surface, SurfaceInstance const& instance, asset::AssetPtr<asset::Material> material);

    ConstIterator begin() const;

    ConstIterator end() const;

    Iterator begin();

    Iterator end();

    void clear();

private:

    std::vector<RenderBatch> _batches;
    std::vector<size_t> _sorted;

};

}