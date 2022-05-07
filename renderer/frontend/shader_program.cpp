// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/shader_program.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

ShaderProgram::ShaderProgram(Context& context, asset::Technique const& technique) :
    _context(&context) {

    for(auto const& shader : technique.shaders()) {
        _shaders.emplace_back(context.device().create_shader(shader.source, get_shader_flags(shader.flags)));
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

    _descriptor_layout = context.device().create_descriptor_layout(bindings);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept {
    _context = other._context;
    _shaders = std::move(other._shaders);
    _uniforms = std::move(other._uniforms);
    for(auto& shader : _shaders) {
        shader = backend::InvalidHandle<backend::Shader>();
    }
    _descriptor_layout = std::exchange(other._descriptor_layout, backend::InvalidHandle<backend::DescriptorLayout>());
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    _context = other._context;
    _shaders = std::move(other._shaders);
    _uniforms = std::move(other._uniforms);
    for(auto& shader : _shaders) {
        shader = backend::InvalidHandle<backend::Shader>();
    }
    _descriptor_layout = std::exchange(other._descriptor_layout, backend::InvalidHandle<backend::DescriptorLayout>());
    return *this;
}

ShaderProgram::~ShaderProgram() {
    for(auto const& shader : _shaders) {
        if(shader != backend::InvalidHandle<backend::Shader>()) {
            _context->device().delete_shader(shader);
        }
    }

    if(_descriptor_layout != backend::InvalidHandle<backend::DescriptorLayout>()) {
        _context->device().delete_descriptor_layout(_descriptor_layout);
    }
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
