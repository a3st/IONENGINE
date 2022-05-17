// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/render_queue.h>

using namespace ionengine;
using namespace ionengine::renderer;

RenderQueue::RenderQueue() {
    
}

void RenderQueue::push(asset::AssetPtr<asset::Mesh> mesh, uint32_t const surface_index, SurfaceInstance const& instance, asset::AssetPtr<asset::Material> material) {

    std::vector<SurfaceInstance> instances;
    instances.emplace_back(instance);

    auto render_batch = RenderBatch {
        .mesh = mesh,
        .surface_index = surface_index,
        .instances = std::move(instances),
        .material = material,
        .sort_index = 0
    };

    _batches.emplace_back(std::move(render_batch));
}

RenderQueue::ConstIterator RenderQueue::begin() const {
    return _batches.begin();
}

RenderQueue::ConstIterator RenderQueue::end() const {
    return _batches.end();
}

RenderQueue::Iterator RenderQueue::begin() {
    return _batches.begin();
}

RenderQueue::Iterator RenderQueue::end() {
    return _batches.end();
}

void RenderQueue::clear() {
    _batches.clear();
}