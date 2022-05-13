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
        _uniforms.insert({ uniform.name, binding_index });

        auto uniform_visitor = make_visitor(
            [&](asset::ShaderUniformData<asset::ShaderUniformType::Sampler2D> const& data) {
                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;
                bindings.emplace_back(binding_index, backend::DescriptorType::Sampler, get_shader_flags(uniform.visibility));
                ++binding_index;
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::CBuffer> const& data) {
                bindings.emplace_back(binding_index, backend::DescriptorType::ConstantBuffer, get_shader_flags(uniform.visibility));
                ++binding_index;
            }
        );

        std::visit(uniform_visitor, uniform.data); 
    }

    _descriptor_layout = device.create_descriptor_layout(bindings);
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
