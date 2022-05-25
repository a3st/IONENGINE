// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_program.h>
#include <lib/math/matrix.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderProgram::ShaderProgram(backend::Device& device, asset::Technique const& technique) :
    _device(&device) {

    bool is_compute = false;

    for(auto const& shader : technique.shaders) {
        if(shader.flags == asset::ShaderFlags::Compute) {
            is_compute = true;
        }
        _shaders.emplace_back(device.create_shader(shader.source, get_shader_flags(shader.flags)));
    }

    std::vector<backend::DescriptorLayoutBinding> bindings;
    uint32_t binding_index = 0;

    for(auto const& uniform : technique.uniforms) {

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

                auto cbuffer_uniform_data = ShaderUniformData<ShaderUniformType::CBuffer> { .index = binding_index };
                uint64_t offset = 0;

                for(auto const& data : data.data) {

                    cbuffer_uniform_data.offsets.insert({ data.name, offset });

                    switch(data.type) {
                        case asset::ShaderDataType::F32: { offset += sizeof(float); } break;
                        case asset::ShaderDataType::F32x2: { offset += sizeof(float) * 2; } break;
                        case asset::ShaderDataType::F32x3: { offset += sizeof(float) * 3; } break;
                        case asset::ShaderDataType::F32x4: { offset += sizeof(lib::math::Vector4f); } break;
                        case asset::ShaderDataType::F32x4x4: { offset += sizeof(lib::math::Matrixf); } break;
                    }
                }

                shader_uniform.data = cbuffer_uniform_data;

                bindings.emplace_back(binding_index, backend::DescriptorType::ConstantBuffer, get_shader_flags(uniform.visibility));
                ++binding_index;
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::SBuffer> const& data) { 

                auto sbuffer_uniform_data = ShaderUniformData<ShaderUniformType::SBuffer> { .index = binding_index };
                uint64_t offset = 0;

                for(auto const& data : data.data) {

                    sbuffer_uniform_data.offsets.insert({ data.name, offset });

                    switch(data.type) {
                        case asset::ShaderDataType::F32: { offset += sizeof(float); } break;
                        case asset::ShaderDataType::F32x2: { offset += sizeof(float) * 2; } break;
                        case asset::ShaderDataType::F32x3: { offset += sizeof(float) * 3; } break;
                        case asset::ShaderDataType::F32x4: { offset += sizeof(lib::math::Vector4f); } break;
                        case asset::ShaderDataType::F32x4x4: { offset += sizeof(lib::math::Matrixf); } break;
                    }
                }

                shader_uniform.data = sbuffer_uniform_data;

                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::RWBuffer> const& data) { 

                auto rwbuffer_uniform_data = ShaderUniformData<ShaderUniformType::RWBuffer> { .index = binding_index };
                uint64_t offset = 0;

                for(auto const& data : data.data) {

                    rwbuffer_uniform_data.offsets.insert({ data.name, offset });

                    switch(data.type) {
                        case asset::ShaderDataType::F32: { offset += sizeof(float); } break;
                        case asset::ShaderDataType::F32x2: { offset += sizeof(float) * 2; } break;
                        case asset::ShaderDataType::F32x3: { offset += sizeof(float) * 3; } break;
                        case asset::ShaderDataType::F32x4: { offset += sizeof(lib::math::Vector4f); } break;
                        case asset::ShaderDataType::F32x4x4: { offset += sizeof(lib::math::Matrixf); } break;
                    }
                }

                shader_uniform.data = rwbuffer_uniform_data;

                bindings.emplace_back(binding_index, backend::DescriptorType::UnorderedAccess, get_shader_flags(uniform.visibility));
                ++binding_index;
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::RWTexture2D> const& data) {

                shader_uniform.data = ShaderUniformData<ShaderUniformType::RWTexture2D> { .index = binding_index };

                bindings.emplace_back(binding_index, backend::DescriptorType::UnorderedAccess, get_shader_flags(uniform.visibility));
                ++binding_index;
            }
        );

        std::visit(uniform_visitor, uniform.data);

        _uniforms.insert({ uniform.name, shader_uniform });
    }

    uint32_t attribute_index = 0;
    uint32_t offset = 0;

    for(auto const& attribute : technique.attributes) {

        std::string const indices = "0123456789";
        std::string semantic_only = attribute.semantic;

        for(auto const& index : indices) {
            if(attribute.semantic.back() == index) {
                semantic_only.pop_back();
                break;
            }
        }
        
        _attributes.emplace_back(semantic_only, attribute_index, get_attribute_format(attribute.type), 0, offset);

        switch(attribute.type) {
            case asset::ShaderDataType::F32: { offset += static_cast<uint32_t>(sizeof(float)); } break;
            case asset::ShaderDataType::F32x2: { offset += static_cast<uint32_t>(sizeof(float) * 2); } break;
            case asset::ShaderDataType::F32x3: { offset += static_cast<uint32_t>(sizeof(float) * 3); } break;
            case asset::ShaderDataType::F32x4: { offset += static_cast<uint32_t>(sizeof(lib::math::Vector4f)); } break;
            case asset::ShaderDataType::F32x4x4: { offset += static_cast<uint32_t>(sizeof(lib::math::Matrixf)); } break;
        }
    }

    _descriptor_layout = device.create_descriptor_layout(bindings, is_compute);
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

std::span<backend::VertexInputDesc const> ShaderProgram::attributes() const {
    return _attributes;
}

uint32_t ShaderProgram::location_by_uniform_name(std::string const& name) const {
    auto it = _uniforms.find(name);
    if(it == _uniforms.end()) {
        throw lib::Exception("Invalid uniform location");
    }

    uint32_t location = 0;

    auto uniform_visitor = make_visitor(
        [&](ShaderUniformData<ShaderUniformType::Sampler2D> const& data) {
            location = data.index;
        },
        [&](ShaderUniformData<ShaderUniformType::CBuffer> const& data) {
            location = data.index;
        },
        [&](ShaderUniformData<ShaderUniformType::SBuffer> const& data) {
            location = data.index;
        },
        [&](ShaderUniformData<ShaderUniformType::RWBuffer> const& data) {
            location = data.index;
        },
        [&](ShaderUniformData<ShaderUniformType::RWTexture2D> const& data) {
            location = data.index;
        }
    );

    std::visit(uniform_visitor, it->second.data);
    return location;
}

std::unordered_map<std::string, ShaderUniform>& ShaderProgram::uniforms() {
    return _uniforms;
}

backend::ShaderFlags constexpr ShaderProgram::get_shader_flags(asset::ShaderFlags const shader_flags) const {
    switch(shader_flags) {
        case asset::ShaderFlags::Vertex: return backend::ShaderFlags::Vertex;
        case asset::ShaderFlags::Pixel: return backend::ShaderFlags::Pixel;
        case asset::ShaderFlags::Geometry: return backend::ShaderFlags::Geometry;
        case asset::ShaderFlags::Domain: return backend::ShaderFlags::Domain;
        case asset::ShaderFlags::Hull: return backend::ShaderFlags::Hull;
        case asset::ShaderFlags::Compute: return backend::ShaderFlags::Compute;
        default: return backend::ShaderFlags::All;
    }
}

backend::Format constexpr ShaderProgram::get_attribute_format(asset::ShaderDataType const data_type) const {
    switch(data_type) {
        case asset::ShaderDataType::F32: return backend::Format::R32;
        case asset::ShaderDataType::F32x2: return backend::Format::RG32;
        case asset::ShaderDataType::F32x3: return backend::Format::RGB32;
        case asset::ShaderDataType::F32x4: return backend::Format::RGBA32;
        default: return backend::Format::Unknown;
    }
}
