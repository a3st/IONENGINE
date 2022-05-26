// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/render_queue.h>

using namespace ionengine;
using namespace ionengine::renderer;

RenderQueue::RenderQueue() {
    
}

void RenderQueue::push(asset::AssetPtr<asset::Mesh> mesh, uint32_t const surface_index, SurfaceInstance const& instance, asset::AssetPtr<asset::Material> material) {

    uint64_t const batch_hash = mesh->surfaces()[surface_index].vertices.hash() ^ mesh->surfaces()[surface_index].indices.hash() ^ material->hash();

    if(_batches_cache.find(batch_hash) == _batches_cache.end()) {

        auto render_batch = RenderBatch {
            .mesh = mesh,
            .surface_index = surface_index,
            .instances = {},
            .material = material,
            .sort_index = material->hash()
        };

        _batches_cache.insert({ batch_hash, _batches.size() });
        _batches.emplace_back(std::move(render_batch));
    }

    auto& batch = _batches.at(_batches_cache.at(batch_hash));
    batch.instances.emplace_back(instance);
}

void RenderQueue::sort() {
    std::sort(_batches.begin(), _batches.end());
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
    _batches_cache.clear();
}
