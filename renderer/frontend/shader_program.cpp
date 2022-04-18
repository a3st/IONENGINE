// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/shader_program.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

backend::ShaderFlags ShaderProgram::get_shader_flags(asset::ShaderType const shader_type) {

    switch(shader_type) {
        case asset::ShaderType::Vertex: return backend::ShaderFlags::Vertex;
        case asset::ShaderType::Pixel: return backend::ShaderFlags::Pixel;
        case asset::ShaderType::Geometry: return backend::ShaderFlags::Geometry;
        case asset::ShaderType::Domain: return backend::ShaderFlags::Domain;
        case asset::ShaderType::Hull: return backend::ShaderFlags::Hull;
        default: return backend::ShaderFlags::All;
    }
}

ShaderProgram::ShaderProgram(Context& context, asset::Technique const& technique) :
    _context(&context) {

    std::for_each(
        technique.shaders().begin(), 
        technique.shaders().end(),
        [&](auto& element) {  
            backend::Handle<backend::Shader> shader = _context->device().create_shader(element.source, get_shader_flags(element.type));
            _shaders.emplace_back(shader);
        }
    );

    std::vector<backend::DescriptorLayoutBinding> bindings;

    std::for_each(
        technique.uniforms().begin(),
        technique.uniforms().end(),
        [&](auto const& element) {

            backend::DescriptorType descriptor_type = {};
            switch(element.type) {
                case asset::ShaderUniformType::CBuffer: descriptor_type = backend::DescriptorType::ConstantBuffer; break;
            }
            
            bindings.emplace_back()
        }
    );

    //_context->device().create_descriptor_layout()
}

ShaderProgram::~ShaderProgram() {

    std::for_each(
        _shaders.begin(), 
        _shaders.end(), 
        [&](auto& element) { 
            _context->device().delete_shader(element); 
        }
    );

    //_context->device().delete_descriptor_layout(_layout);
}