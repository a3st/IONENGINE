// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/shader_cache.h>

namespace ionengine::renderer {

class PipelineCache {
public:

    PipelineCache(backend::Device& device);

    PipelineCache(PipelineCache const&) = delete;

    PipelineCache(PipelineCache&& other) noexcept;

    PipelineCache& operator=(PipelineCache const&) = delete;

    PipelineCache& operator=(PipelineCache&& other) noexcept;

    backend::Handle<backend::Pipeline> get(Shader& shader, backend::Handle<backend::RenderPass> const& render_pass = backend::InvalidHandle<backend::RenderPass>());

private:

    backend::Device* _device;

    std::mutex _mutex;

    std::unordered_map<uint32_t, backend::Handle<backend::Pipeline>> _data;
};

}
