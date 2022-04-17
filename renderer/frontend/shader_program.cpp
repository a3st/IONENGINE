// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/shader_program.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

backend::ShaderFlags ShaderProgram::get_shader_flags(ShaderType const shader_type) {

    switch(shader_type) {
        case ShaderType::vertex: return backend::ShaderFlags::Vertex;
        case ShaderType::pixel: return backend::ShaderFlags::Pixel;
        case ShaderType::geometry: return backend::ShaderFlags::Geometry;
        case ShaderType::domain: return backend::ShaderFlags::Domain;
        case ShaderType::hull: return backend::ShaderFlags::Hull;
        default: return backend::ShaderFlags::All;
    }
}

ShaderProgram::ShaderProgram(Context& context, asset::Technique const& technique) :
    _context(&context) {

    std::for_each(
        technique.data().shaders.begin(), 
        technique.data().shaders.end(),
        [&](auto& element) {  
            backend::Handle<backend::Shader> shader = _context->device().create_shader(element.source, get_shader_flags(element.type));
            _shaders.emplace_back(shader);
        }
    );
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