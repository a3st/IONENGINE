// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/shader_cache.h>
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

    std::tuple<std::shared_ptr<Pipeline>, std::shared_ptr<ShaderProgram>> get(
        ShaderCache& shader_cache,
        asset::Technique& technique,
        asset::MaterialPassParameters const& parameters,
        backend::Handle<backend::RenderPass> const& render_pass
    );

    std::tuple<std::shared_ptr<Pipeline>, std::shared_ptr<ShaderProgram>> get(
        ShaderCache& shader_cache,
        asset::Technique& technique
    );

private:

    backend::Device* _device;

    std::unordered_map<uint32_t, std::shared_ptr<Pipeline>> _data;
};

}