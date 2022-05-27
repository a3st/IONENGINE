// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/shader_cache.h>
#include <asset/material.h>

namespace ionengine::renderer {

class PipelineCache {
public:

    PipelineCache(backend::Device& device);

    PipelineCache(PipelineCache const&) = delete;

    PipelineCache(PipelineCache&& other) noexcept;

    PipelineCache& operator=(PipelineCache const&) = delete;

    PipelineCache& operator=(PipelineCache&& other) noexcept;

    std::tuple<backend::Handle<backend::Pipeline>, ResourcePtr<ShaderProgram>> get(
        ShaderCache& shader_cache,
        asset::Technique& technique,
        backend::Handle<backend::RenderPass> const& render_pass
    );

    std::tuple<backend::Handle<backend::Pipeline>, ResourcePtr<ShaderProgram>> get(
        ShaderCache& shader_cache,
        asset::Technique& technique
    );

private:

    backend::Device* _device;

    std::unordered_map<uint32_t, backend::Handle<backend::Pipeline>> _data;
};

backend::FillMode constexpr get_fill_mode(asset::FillMode const fill_mode);

backend::CullMode constexpr get_cull_mode(asset::CullMode const cull_mode);

}
