// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/shader_program.h>
#include <asset/material.h>

namespace ionengine::renderer {
    
class Pipeline {
public:

    ~Pipeline();

    static std::shared_ptr<Pipeline> from_data(
        backend::Device& device,
        std::shared_ptr<ShaderProgram> shader_program,
        asset::MaterialPassParameters const& parameters,
        backend::Handle<backend::RenderPass> const& render_pass
    );

    void bind(backend::Handle<backend::CommandList> const& command_list);

    std::shared_ptr<ShaderProgram> shader_program() const;

private:

    Pipeline(
        backend::Device& device,
        std::span<backend::VertexInputDesc const> const vertex_descs,
        std::shared_ptr<ShaderProgram> shader_program,
        asset::MaterialPassParameters const& parameters,
        backend::Handle<backend::RenderPass> const& render_pass
    );

    backend::Device* _device;

    backend::Handle<backend::Pipeline> _pipeline;

    std::shared_ptr<ShaderProgram> _shader_program;

    backend::FillMode constexpr get_fill_mode(asset::MaterialPassFillMode const fill_mode) const;

    backend::CullMode constexpr get_cull_mode(asset::MaterialPassCullMode const cull_mode) const;

};

}