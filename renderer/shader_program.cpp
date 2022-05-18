// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_program.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderProgram::ShaderProgram(backend::Device& device, asset::Technique const& technique) :
    _device(&device) {

    for(auto const& shader : technique.shaders()) {
        _shaders.emplace_back(device.create_shader(shader.source, get_shader_flags(shader.flags)));
    }

    std::vector<backend::DescriptorLayoutBinding> bindings;
    uint32_t binding_index = 0;

    for(auto const& uniform : technique.uniforms()) {

        auto shader_uniform = ShaderUniform { };

        auto uniform_visitor = make_visitor(
            [&](asset::ShaderUniformData<asset::ShaderUniformType::Sampler2D> const& data) {

                shader_uniform.data = ShaderUniformData<ShaderUniformType::Sampler2D> { .index = binding_index };

                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;
                bindings.emplace_back(binding_index, backend::DescriptorType::Sampler, get_shader_flags(uniform.visibility));
                ++binding_index;
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::CBuffer> const& data) {

                auto cbuffer_uniform = ShaderUniformData<ShaderUniformType::CBuffer> { .index = binding_index };

                for(auto const& data : data.data) {
                    
                    cbuffer_uniform.data_offset.insert({ data.name, 0 });
                }

                shader_uniform.data = cbuffer_uniform;

                bindings.emplace_back(binding_index, backend::DescriptorType::ConstantBuffer, get_shader_flags(uniform.visibility));
                ++binding_index;
            }
        );

        std::visit(uniform_visitor, uniform.data);

        _uniforms.insert({ uniform.name, shader_uniform });
    }

    _descriptor_layout = device.create_descriptor_layout(bindings);
}

std::shared_ptr<ShaderProgram> ShaderProgram::from_technique(backend::Device& device, asset::Technique const& technique) {
    return std::shared_ptr<ShaderProgram>(new ShaderProgram(device, technique));
}

ShaderProgram::~ShaderProgram() {
    for(auto const& shader : _shaders) {
        _device->delete_shader(shader);
    }
    _device->delete_descriptor_layout(_descriptor_layout);
}

backend::Handle<backend::DescriptorLayout> ShaderProgram::descriptor_layout() const { 
    return _descriptor_layout; 
}

std::span<backend::Handle<backend::Shader> const> ShaderProgram::shaders() const { 
    return _shaders;
}

uint32_t ShaderProgram::location_by_uniform_name(std::string const& name) const {
    auto it = _uniforms.find(name);
    if(it == _uniforms.end()) {
        throw lib::Exception("Invalid uniform location");
    }
    return it->second;
}

std::unordered_map<std::string, uint32_t>& ShaderProgram::uniforms() {
    return _uniforms;
}

backend::ShaderFlags constexpr ShaderProgram::get_shader_flags(asset::ShaderFlags const shader_flags) const {
    switch(shader_flags) {
        case asset::ShaderFlags::Vertex: return backend::ShaderFlags::Vertex;
        case asset::ShaderFlags::Pixel: return backend::ShaderFlags::Pixel;
        case asset::ShaderFlags::Geometry: return backend::ShaderFlags::Geometry;
        case asset::ShaderFlags::Domain: return backend::ShaderFlags::Domain;
        case asset::ShaderFlags::Hull: return backend::ShaderFlags::Hull;
        default: return backend::ShaderFlags::All;
    }
}
