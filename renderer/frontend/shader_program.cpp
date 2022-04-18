// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/shader_program.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;


backend::ShaderFlags ShaderProgram::get_shader_flags(asset::ShaderFlags const shader_flags) {

    switch(shader_flags) {
        case asset::ShaderFlags::Vertex: return backend::ShaderFlags::Vertex;
        case asset::ShaderFlags::Pixel: return backend::ShaderFlags::Pixel;
        case asset::ShaderFlags::Geometry: return backend::ShaderFlags::Geometry;
        case asset::ShaderFlags::Domain: return backend::ShaderFlags::Domain;
        case asset::ShaderFlags::Hull: return backend::ShaderFlags::Hull;
        default: return backend::ShaderFlags::All;
    }
}


ShaderProgram::ShaderProgram(Context& context, asset::Technique const& technique) :
    _context(&context) {

    std::for_each(
        technique.data().begin(), 
        technique.data().end(),
        [&](auto& element) {  
            backend::Handle<backend::Shader> shader = _context->device().create_shader(element.source, get_shader_flags(element.flags));
            _shaders.emplace_back(shader);

            std::cout << 123 << std::endl;
        }
    );

    std::vector<backend::DescriptorLayoutBinding> bindings;

    std::for_each(
        technique.uniforms().begin(),
        technique.uniforms().end(),
        [&](auto const& element) {

            
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