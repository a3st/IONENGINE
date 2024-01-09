// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/mesh.h>
#include <asset/material.h>
#include <lib/math/matrix.h>

namespace ionengine::renderer {

struct SurfaceInstance {
    lib::math::Matrixf model;
    lib::math::Matrixf inverse_model;
};

struct RenderBatch {
    asset::AssetPtr<asset::Mesh> mesh;
    uint32_t surface_index;
    std::vector<SurfaceInstance> instances;
    asset::AssetPtr<asset::Material> material;
    uint64_t sort_index;

    bool operator<(RenderBatch const& other) const {
        return sort_index < other.sort_index;
    }
};

class RenderQueue {
public:

    using ConstIterator = std::vector<RenderBatch>::const_iterator;

    using Iterator = std::vector<RenderBatch>::iterator;

    RenderQueue();

    void push(asset::AssetPtr<asset::Mesh> mesh, uint32_t const surface_index, SurfaceInstance const& instance, asset::AssetPtr<asset::Material> material);

    ConstIterator begin() const;

    ConstIterator end() const;

    Iterator begin();

    Iterator end();

    void clear();

    void sort();

private:

    std::unordered_map<uint64_t, size_t> _batches_cache;
    std::vector<RenderBatch> _batches;
};

}