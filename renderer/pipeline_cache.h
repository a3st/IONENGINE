// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/pipeline.h>
#include <asset/material.h>

namespace ionengine::renderer {

class PipelineCache {
public:

    PipelineCache(backend::Device& device);

    PipelineCache(PipelineCache const&) = delete;

    PipelineCache(PipelineCache&& other) noexcept;

    PipelineCache& operator=(PipelineCache const&) = delete;

    PipelineCache& operator=(PipelineCache&& other) noexcept;

    std::shared_ptr<Pipeline> get(asset::Material& material, backend::Handle<backend::RenderPass> const& render_pass);

private:

    backend::Device* _device;

    lib::SparseVector<CacheEntry<std::shared_ptr<Pipeline>>> _data;
};

}