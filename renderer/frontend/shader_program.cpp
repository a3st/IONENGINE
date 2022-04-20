// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/shader_program.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

ShaderProgram::ShaderProgram(Context& context, asset::Technique const& technique) :
    _context(&context) {

    std::for_each(
        technique.shaders().begin(), 
        technique.shaders().end(),
        [&](auto const& shader_data) {  
            backend::Handle<backend::Shader> compiled_shader = _context->device().create_shader(shader_data.source, get_shader_flags(shader_data.flags));
            _shaders.emplace_back(compiled_shader);
        }
    );

    std::vector<backend::DescriptorLayoutBinding> bindings;

    uint32_t binding = 0;

    std::for_each(
        technique.uniforms().begin(),
        technique.uniforms().end(),
        [&](auto const& element) {

            _uniforms.insert({ element.name, binding });

            std::cout << binding << " " << element.name << " " << std::boolalpha << element.is_export << std::endl;

            auto uniform_visitor = make_visitor(
                [&](asset::ShaderUniformData<asset::ShaderUniformType::Sampler2D> const& data) {
                    bindings.emplace_back(binding, backend::DescriptorType::ShaderResource, get_shader_flags(element.visibility));
                    ++binding;
                    bindings.emplace_back(binding, backend::DescriptorType::Sampler, get_shader_flags(element.visibility));
                },
                [&](asset::ShaderUniformData<asset::ShaderUniformType::CBuffer> const& data) {
                    bindings.emplace_back(binding, backend::DescriptorType::ConstantBuffer, get_shader_flags(element.visibility));
                }
            );

            std::visit(uniform_visitor, element.data);
            ++binding;
        }
    );

    _layout = _context->device().create_descriptor_layout(bindings);

    descriptor_sets[0] = _context->device().create_descriptor_set(_layout);
    descriptor_sets[1] = _context->device().create_descriptor_set(_layout);
}

ShaderProgram::~ShaderProgram() {

    std::for_each(
        _shaders.begin(), 
        _shaders.end(), 
        [&](auto const& shader) { 
            _context->device().delete_shader(shader); 
        }
    );

    _context->device().delete_descriptor_layout(_layout);
}

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
