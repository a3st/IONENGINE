// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/gpu_buffer.h>
#include <renderer/gpu_texture.h>
#include <asset/technique.h>

namespace ionengine::renderer {

enum class ShaderUniformType {
    CBuffer,
    Sampler2D
};

template<ShaderUniformType Type>
struct ShaderUniformData { };

template<>
struct ShaderUniformData<ShaderUniformType::Sampler2D> { 
    uint32_t index;
};

template<>
struct ShaderUniformData<ShaderUniformType::CBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> data_offset; 
};

struct ShaderUniform {
    std::variant<
        ShaderUniformData<ShaderUniformType::Sampler2D>,
        ShaderUniformData<ShaderUniformType::CBuffer>
    > data;
};

class ShaderProgram {
public:

    ~ShaderProgram();

    static std::shared_ptr<ShaderProgram> from_technique(backend::Device& device, asset::Technique const& technique);

    backend::Handle<backend::DescriptorLayout> descriptor_layout() const;

    std::span<backend::Handle<backend::Shader> const> shaders() const;

    uint32_t location_by_uniform_name(std::string const& name) const;

    std::unordered_map<std::string, ShaderUniform>& uniforms();

private:

    ShaderProgram(backend::Device& device, asset::Technique const& technique);

    backend::Device* _device;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _descriptor_layout;

    std::unordered_map<std::string, ShaderUniform> _uniforms;

    backend::ShaderFlags constexpr get_shader_flags(asset::ShaderFlags const shader_flags) const;
};

class ShaderUniformBinder {
public:

    ShaderUniformBinder(backend::Device& device, ShaderProgram& shader_program) :
        _device(&device), _shader_program(&shader_program) {
    }

    void bind_cbuffer(uint32_t const index, GPUBuffer& buffer) {
        if(_updates.find(index) == _updates.end()) {
            _updates.insert({ index, static_cast<uint32_t>(_descriptor_writes.size()) });
            _descriptor_writes.emplace_back(index, buffer.as_buffer());
        } else {
            _descriptor_writes.at(_updates.at(index)) = backend::DescriptorWriteDesc { .index = index, .data = buffer.as_buffer() };
        }
    }

    void bind_texture(uint32_t const index, GPUTexture& texture) {
        if(_updates.find(index) == _updates.end()) {
            _updates.insert({ index, static_cast<uint32_t>(_descriptor_writes.size()) });
            _descriptor_writes.emplace_back(index, texture.as_texture());
            _descriptor_writes.emplace_back(index + 1, texture.as_sampler());
        } else {
            _descriptor_writes.at(_updates.at(index)) = backend::DescriptorWriteDesc { .index = index, .data = texture.as_texture() };
            _descriptor_writes.at(_updates.at(index) + 1) = backend::DescriptorWriteDesc { .index = index + 1, .data = texture.as_sampler() };
        }
    }

    void update(backend::Handle<backend::CommandList> const& command_list) {
        _device->bind_resources(command_list, _shader_program->descriptor_layout(), _descriptor_writes);
    }

private:

    backend::Device* _device;
    ShaderProgram* _shader_program;

    std::unordered_map<uint32_t, uint32_t> _updates;
    std::vector<backend::DescriptorWriteDesc> _descriptor_writes;
};

}
