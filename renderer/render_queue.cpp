// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/render_queue.h>

using namespace ionengine;
using namespace ionengine::renderer;

RenderQueue::RenderQueue() {
    
}

void RenderQueue::push(std::shared_ptr<asset::Surface> surface, SurfaceInstance const& instance, asset::AssetPtr<asset::Material> material) {

    std::vector<SurfaceInstance> instances;
    instances.emplace_back(instance);

    auto render_batch = RenderBatch {
        .surface = surface,
        .instances = std::move(instances),
        .material = material,
        .sort_index = 0
    };

    _batches.emplace_back(std::move(render_batch));
}