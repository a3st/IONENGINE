// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <asset/material.h>

namespace ionengine::renderer {
    
class Pipeline {
public:

    ~Pipeline();

    static std::shared_ptr<Pipeline> from_data(
        backend::Device& device,
        backend::Handle<backend::DescriptorLayout> const& descriptor_layout,
        std::span<backend::Handle<backend::Shader> const> const shaders,
        asset::MaterialPassParameters const& parameters,
        backend::Handle<backend::RenderPass> const& render_pass
    );

    void bind(backend::Handle<backend::CommandList> const& command_list);

private:

    Pipeline(
        backend::Device& device,
        backend::Handle<backend::DescriptorLayout> const& descriptor_layout,
        std::span<backend::VertexInputDesc const> const vertex_descs,
        std::span<backend::Handle<backend::Shader> const> const shaders,
        asset::MaterialPassParameters const& parameters,
        backend::Handle<backend::RenderPass> const& render_pass
    );

    backend::Device* _device;

    backend::Handle<backend::Pipeline> _pipeline;

    backend::FillMode constexpr get_fill_mode(asset::MaterialPassFillMode const fill_mode) const;

    backend::CullMode constexpr get_cull_mode(asset::MaterialPassCullMode const cull_mode) const;

};

}