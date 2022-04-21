// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <asset/technique.h>

namespace ionengine::renderer::frontend {

class ShaderProgram {

    friend class ShaderUniformBinder;

public:

    ShaderProgram(Context& context, asset::Technique const& technique);

    ~ShaderProgram();

    backend::Handle<backend::DescriptorLayout> layout() const;

    std::span<backend::Handle<backend::Shader> const> shaders() const;

    uint32_t get_uniform_by_name(std::string const& name) const;

private:

    Context* _context;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _layout;

    std::unordered_map<std::string, uint32_t> _uniforms;

    std::array<backend::Handle<backend::DescriptorSet>, 2> descriptor_sets;

    backend::ShaderFlags get_shader_flags(asset::ShaderFlags const shader_flags);
};

class ShaderUniformBinder {
public:

    ShaderUniformBinder(Context& context, ShaderProgram& program);

    void bind_cbuffer(uint32_t const index, backend::Handle<backend::Buffer> const& buffer);

    void bind_texture(uint32_t const index, backend::Handle<backend::Texture> const& texture, backend::Handle<backend::Sampler> const& sampler);

    void update(backend::Handle<backend::CommandList> const& command_list);

private:

    Context* _context;
    ShaderProgram* _program;

    std::vector<backend::DescriptorWriteDesc> _descriptor_writes;
};

}
