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

    std::cout << std::format("----- {} ------", technique.name()) << std::endl;

    std::for_each(
        technique.uniforms().begin(),
        technique.uniforms().end(),
        [&](auto const& element) {

            _uniforms.insert({ element.name, binding });

            std::cout << std::format("Binding: {}, Name: {}", binding, element.name) << std::endl;

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

backend::Handle<backend::DescriptorLayout> ShaderProgram::layout() const { 
    return _layout; 
}

std::span<backend::Handle<backend::Shader> const> ShaderProgram::shaders() const { 
    return _shaders; 
}

uint32_t ShaderProgram::get_uniform_by_name(std::string const& name) const {

    return _uniforms.at(name);
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

ShaderUniformBinder::ShaderUniformBinder(Context& context, ShaderProgram& program) : 
    _context(&context), _program(&program) {

}

void ShaderUniformBinder::bind_cbuffer(uint32_t const index, backend::Handle<backend::Buffer> const& buffer) {

    _descriptor_writes.emplace_back(index, buffer);
}

void ShaderUniformBinder::bind_texture(uint32_t const index, backend::Handle<backend::Texture> const& texture, backend::Handle<backend::Sampler> const& sampler) {

    _descriptor_writes.emplace_back(index, texture);
    _descriptor_writes.emplace_back(index + 1, sampler);
}

void ShaderUniformBinder::update(backend::Handle<backend::CommandList> const& command_list) {

    _context->device().update_descriptor_set(_program->descriptor_sets.at(_context->_frame_index), _descriptor_writes);
    _context->device().bind_descriptor_set(command_list, _program->descriptor_sets.at(_context->_frame_index));
}
