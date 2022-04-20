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

    backend::Handle<backend::DescriptorLayout> layout() const { return _layout; }

    std::span<backend::Handle<backend::Shader> const> shaders() const { return _shaders; }

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

    ShaderUniformBinder(Context& context, ShaderProgram& program) : 
        _context(&context), _program(&program) {

    }

    uint32_t get_uniform_by_name(std::string const& name) const {

        return _program->_uniforms.at(name);
    }

    ShaderUniformBinder& bind_cbuffer(uint32_t const index, backend::Handle<backend::Buffer> const& buffer) {

        _descriptor_writes.emplace_back(index, buffer);
        return *this;
    }

    ShaderUniformBinder& bind_sampler(uint32_t const index, backend::Handle<backend::Texture> const& texture, backend::Handle<backend::Sampler> const& sampler) {

        _descriptor_writes.emplace_back(index, texture);
        _descriptor_writes.emplace_back(index + 1, sampler);
        return *this;
    }

    void update(backend::Handle<backend::CommandList> const& command_list) {

        _context->device().update_descriptor_set(_program->descriptor_sets.at(_context->_frame_index), _descriptor_writes);
        _context->device().bind_descriptor_set(command_list, _program->descriptor_sets.at(_context->_frame_index));
    }

private:

    Context* _context;
    ShaderProgram* _program;

    std::vector<backend::DescriptorWriteDesc> _descriptor_writes;
};

}