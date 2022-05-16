// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/gpu_buffer.h>
#include <renderer/gpu_texture.h>
#include <asset/technique.h>

namespace ionengine::renderer {

class ShaderProgram {
public:

    ~ShaderProgram();

    static std::shared_ptr<ShaderProgram> from_technique(backend::Device& device, asset::Technique const& technique);

    backend::Handle<backend::DescriptorLayout> descriptor_layout() const;

    std::span<backend::Handle<backend::Shader> const> shaders() const;

    uint32_t location_by_uniform_name(std::string const& name) const;

private:

    ShaderProgram(backend::Device& device, asset::Technique const& technique);

    backend::Device* _device;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _descriptor_layout;

    std::unordered_map<std::string, uint32_t> _uniforms;

    backend::ShaderFlags constexpr get_shader_flags(asset::ShaderFlags const shader_flags) const;
};

class ShaderUniformBinder {
public:

    ShaderUniformBinder(backend::Device& device, ShaderProgram& shader_program) :
        _device(&device), _shader_program(&shader_program) {

    }

    void bind_cbuffer(std::string const& uniform_name, GPUBuffer& buffer) {
        uint32_t const uniform_location = _shader_program->location_by_uniform_name(uniform_name);
        _descriptor_writes.emplace_back(uniform_location, buffer.as_buffer());
    }

    void bind_texture(std::string const& uniform_name, GPUTexture& texture) {
        uint32_t const uniform_location = _shader_program->location_by_uniform_name(uniform_name);
        _descriptor_writes.emplace_back(uniform_location, texture.as_texture());
        _descriptor_writes.emplace_back(uniform_location + 1, texture.as_sampler());
    }

    void update(backend::Handle<backend::CommandList> const& command_list) {
        _device->bind_resources(command_list, _shader_program->descriptor_layout(), _descriptor_writes);
    }

private:

    backend::Device* _device;
    ShaderProgram* _shader_program;

    std::vector<backend::DescriptorWriteDesc> _descriptor_writes;
};

}
