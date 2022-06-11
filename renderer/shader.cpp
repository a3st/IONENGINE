// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader.h>
#include <lib/math/matrix.h>
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<Shader, lib::Result<ShaderError>> Shader::load_from_file(std::filesystem::path const& shader_path, backend::Device& device) {

    std::string const path_string = shader_path.string();

    auto document = JSON_ShaderDefinition {};
    json5::error const result = json5::from_file(path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::make_expected<Shader, lib::Result<ShaderError>>(
            lib::Result<ShaderError> { .errc = ShaderError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::make_expected<Shader, lib::Result<ShaderError>>(
            lib::Result<ShaderError> { .errc = ShaderError::ParseError, .message = std::format("Parse file error '{}'", shader_path.string()) }
        );
    }

    auto shader = Shader {};
    shader.name = document.name;

    std::unordered_map<std::string, uint32_t> locations;
    std::unordered_map<backend::DescriptorType, uint32_t> registers_count;
    registers_count.insert({ backend::DescriptorType::ShaderResource, 0 });
    registers_count.insert({ backend::DescriptorType::ConstantBuffer, 0 });
    registers_count.insert({ backend::DescriptorType::UnorderedAccess, 0 });
    registers_count.insert({ backend::DescriptorType::Sampler, 0 });

    std::vector<backend::DescriptorLayoutBinding> bindings;
    uint32_t binding_index = 0;

    for(auto const& uniform : document.uniforms) {

        auto shader_uniform = ShaderUniform {};

        switch(uniform.type) {

            case JSON_ShaderUniformType::sbuffer: {
                auto sbuffer_data = ShaderUniformData<ShaderUniformType::SBuffer> { .index = binding_index };
                
                calculate_uniform_offsets(uniform.properties.value(), sbuffer_data.offsets);
                shader_uniform.data = std::move(sbuffer_data);

                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;

                locations.insert({ uniform.name, registers_count.at(backend::DescriptorType::ShaderResource) });
                ++registers_count.at(backend::DescriptorType::ShaderResource);
            } break;

            case JSON_ShaderUniformType::rwbuffer: {
                auto rwbuffer_data = ShaderUniformData<ShaderUniformType::RWBuffer> { .index = binding_index };
                
                calculate_uniform_offsets(uniform.properties.value(), rwbuffer_data.offsets);
                shader_uniform.data = std::move(rwbuffer_data);

                bindings.emplace_back(binding_index, backend::DescriptorType::UnorderedAccess, get_shader_flags(uniform.visibility));
                ++binding_index;

                locations.insert({ uniform.name, registers_count.at(backend::DescriptorType::UnorderedAccess) });
                ++registers_count.at(backend::DescriptorType::UnorderedAccess);
            } break;

            case JSON_ShaderUniformType::cbuffer: {
                auto cbuffer_data = ShaderUniformData<ShaderUniformType::CBuffer> { .index = binding_index };

                calculate_uniform_offsets(uniform.properties.value(), cbuffer_data.offsets);
                shader_uniform.data = std::move(cbuffer_data);

                bindings.emplace_back(binding_index, backend::DescriptorType::ConstantBuffer, get_shader_flags(uniform.visibility));
                ++binding_index;

                locations.insert({ uniform.name, registers_count.at(backend::DescriptorType::ConstantBuffer) });
                ++registers_count.at(backend::DescriptorType::ConstantBuffer);
            } break;

            case JSON_ShaderUniformType::rwtexture2D: {
                shader_uniform.data = ShaderUniformData<ShaderUniformType::RWTexture2D> { .index = binding_index };

                bindings.emplace_back(binding_index, backend::DescriptorType::UnorderedAccess, get_shader_flags(uniform.visibility));
                ++binding_index;

                locations.insert({ uniform.name, registers_count.at(backend::DescriptorType::UnorderedAccess) });
                ++registers_count.at(backend::DescriptorType::UnorderedAccess);
            } break;

            case JSON_ShaderUniformType::sampler2D: {
                shader_uniform.data = ShaderUniformData<ShaderUniformType::Sampler2D> { .index = binding_index };

                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;
                bindings.emplace_back(binding_index, backend::DescriptorType::Sampler, get_shader_flags(uniform.visibility));
                ++binding_index;

                locations.insert({ uniform.name, registers_count.at(backend::DescriptorType::ShaderResource) });
                ++registers_count.at(backend::DescriptorType::ShaderResource);
                locations.insert({ uniform.name, registers_count.at(backend::DescriptorType::Sampler) });
                ++registers_count.at(backend::DescriptorType::Sampler);
            } break;
        }

        shader.uniforms.insert({ uniform.name, std::move(shader_uniform) });
    }

    shader.rasterizer = backend::RasterizerDesc {
        .fill_mode = get_fill_mode(document.draw_parameters.fill_mode),
        .cull_mode = get_cull_mode(document.draw_parameters.cull_mode)
    };

    shader.depth_stencil = backend::DepthStencilDesc {
        .depth_func = backend::CompareOp::Less,
        .write_enable = document.draw_parameters.depth_stencil
    };

    shader.blend = backend::BlendDesc { 
        document.draw_parameters.blend,
        backend::Blend::SrcAlpha, backend::Blend::InvSrcAlpha, backend::BlendOp::Add, 
        backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add 
    };

    for(auto const& stage : document.stages) {

        std::string shader_code = "";

        switch(stage.type) {

            case JSON_ShaderType::vertex: {
                shader_code += generate_struct_code("vs_input", stage.inputs);
                shader_code += generate_struct_code("vs_output", stage.outputs);

                uint32_t attribute_index = 0;
                uint32_t offset = 0;

                for(auto const& attribute : stage.inputs) {

                    std::string const indices = "0123456789";
                    std::string semantic_only = attribute.semantic.value();

                    for(auto const& index : indices) {
                        if(attribute.semantic.value().back() == index) {
                            semantic_only.pop_back();
                            break;
                        }
                    }

                    shader.attributes.emplace_back(semantic_only, attribute_index, get_attribute_format(attribute.type), 0, offset);
                    offset += static_cast<uint32_t>(get_shader_data_type_size(attribute.type));
                }

            } break;

            case JSON_ShaderType::pixel: {
                shader_code += generate_struct_code("ps_input", stage.inputs);
                shader_code += generate_struct_code("ps_output", stage.outputs);
            } break;

            // TODO! Hull, Domain and Geometry
        }

        for(auto& uniform : document.uniforms) {

            if(uniform.visibility != stage.type && uniform.visibility != JSON_ShaderType::all) {
                continue;
            }

            if(uniform.properties.has_value()) {
                shader_code += generate_uniform_code(uniform.name, uniform.type, locations.at(uniform.name), uniform.properties.value());
            } else {
                shader_code += generate_uniform_code(uniform.name, uniform.type, locations.at(uniform.name));
            }
        }

        shader_code += stage.source;
        shader.stages.emplace_back(device.create_shader(shader_code, get_shader_flags(stage.type)));
    }

    shader.descriptor_layout = device.create_descriptor_layout(bindings, false);

    shader.hash = XXHash64::hash(shader.name.data(), shader.name.size(), 0);

    return lib::make_expected<Shader, lib::Result<ShaderError>>(std::move(shader));
}

uint32_t Shader::location_uniform_by_name(std::string_view const name) const {
    auto it = uniforms.find(std::string(name.begin(), name.end()));
    assert(it != uniforms.end() && "invalid uniform location");

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

void ionengine::renderer::calculate_uniform_offsets(
    std::span<JSON_ShaderStructDefinition const> const properties,
    std::unordered_map<std::string, uint64_t>& offsets
) {
    uint64_t offset = 0;

    for(auto const& property : properties) {
        offsets.insert({ property.name, offset });
        offset += get_shader_data_type_size(property.type);
    }
}

std::string ionengine::renderer::generate_uniform_code(
    std::string_view const name, 
    JSON_ShaderUniformType const uniform_type, 
    uint32_t const location,
    std::optional<std::span<JSON_ShaderStructDefinition const>> const properties
) {

    std::string generated_code = "";

    switch(uniform_type) {

        case JSON_ShaderUniformType::cbuffer: {
            std::string const struct_name = std::format("{}_data", name);
            generated_code += generate_struct_code(struct_name, properties.value());
            generated_code += std::format("ConstantBuffer<{}> {} : register(b{});\n", struct_name, name, location);
        } break;

        case JSON_ShaderUniformType::sbuffer: {
            std::string const struct_name = std::format("{}_data", name);
            generated_code += generate_struct_code(struct_name, properties.value());
            generated_code += std::format("StructuredBuffer<{}> {}s : register(t{});\n", struct_name, name, location);
        } break;

        case JSON_ShaderUniformType::rwbuffer: {
            std::string const struct_name = std::format("{}_data", name);
            generated_code += generate_struct_code(struct_name, properties.value());
            generated_code += std::format("RWStructuredBuffer<{}> {}s : register(u{});\n", struct_name, name, location);
        } break;

        case JSON_ShaderUniformType::sampler2D: {
            generated_code += std::format("SamplerState {}_sampler : register(s{}); ", name, location);
            generated_code += std::format("Texture2D {}_texture : register(t{});\n", name, location);
        } break;

        case JSON_ShaderUniformType::rwtexture2D: {
            generated_code += std::format("RWTexture2D<float> {}_texture : register(u{});\n", name, location);
        } break;

        default: {
            assert(false && "invalid data type");
        } break;
    }
    return generated_code;
}

std::string ionengine::renderer::generate_struct_code(
    std::string_view const name, 
    std::span<JSON_ShaderStructDefinition const> const properties
) {
    std::string generated_code = "struct";
    generated_code += std::format(" {} {{ ", name);
    for(auto& property : properties) {
        if(property.semantic.has_value()) {
            generated_code += std::format("{} {} : {}; ", get_shader_data_type_string(property.type), property.name, property.semantic.value());
        } else {
            generated_code += std::format("{} {}; ", get_shader_data_type_string(property.type), property.name);
        }
    }
    generated_code += "};\n";
    return generated_code;
}

backend::ShaderFlags constexpr ionengine::renderer::get_shader_flags(JSON_ShaderType const shader_type) {
    switch(shader_type) {
        case JSON_ShaderType::vertex: return backend::ShaderFlags::Vertex;
        case JSON_ShaderType::pixel: return backend::ShaderFlags::Pixel;
        case JSON_ShaderType::geometry: return backend::ShaderFlags::Geometry;
        case JSON_ShaderType::domain: return backend::ShaderFlags::Domain;
        case JSON_ShaderType::hull: return backend::ShaderFlags::Hull;
        case JSON_ShaderType::compute: return backend::ShaderFlags::Compute;
        case JSON_ShaderType::all: return backend::ShaderFlags::All;
        default: {
            assert(false && "invalid data type");
            return backend::ShaderFlags::All;
        }
    }
}

backend::Format constexpr ionengine::renderer::get_attribute_format(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::_float: return backend::Format::R32;
        case JSON_ShaderDataType::float2: return backend::Format::RG32;
        case JSON_ShaderDataType::float3: return backend::Format::RGB32;
        case JSON_ShaderDataType::float4: return backend::Format::RGBA32;
        default: {
            assert(false && "invalid data type");
            return backend::Format::Unknown;
        }
    }
}

std::string constexpr ionengine::renderer::get_shader_data_type_string(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::float4x4: return "float4x4";
        case JSON_ShaderDataType::float4: return "float4";
        case JSON_ShaderDataType::float3: return "float3";
        case JSON_ShaderDataType::float2: return "float2";
        case JSON_ShaderDataType::_float: return "float";
        case JSON_ShaderDataType::_uint: return "uint";
        case JSON_ShaderDataType::_bool: return "bool";
        default: {
            assert(false && "invalid data type");
            return "float";
        }
    }
}

uint64_t constexpr ionengine::renderer::get_shader_data_type_size(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::_float: return sizeof(float);
        case JSON_ShaderDataType::float2: return sizeof(float) * 2;
        case JSON_ShaderDataType::float3: return sizeof(float) * 3;
        case JSON_ShaderDataType::float4: return sizeof(lib::math::Vector4f);
        case JSON_ShaderDataType::float4x4: return sizeof(lib::math::Matrixf);
        case JSON_ShaderDataType::_uint: return sizeof(uint32_t);
        case JSON_ShaderDataType::_bool: return sizeof(bool);
        default: {
            assert(false && "invalid data type");
            return 0;
        }
    }
}

backend::FillMode constexpr ionengine::renderer::get_fill_mode(JSON_ShaderFillMode const fill_mode) {
    switch(fill_mode) {
        case JSON_ShaderFillMode::solid: return backend::FillMode::Solid;
        case JSON_ShaderFillMode::wireframe: return backend::FillMode::Wireframe;
        default: {
            assert(false && "invalid data type");
            return backend::FillMode::Solid;
        } 
    }
}

backend::CullMode constexpr ionengine::renderer::get_cull_mode(JSON_ShaderCullMode const cull_mode) {
    switch(cull_mode) {
        case JSON_ShaderCullMode::front: return backend::CullMode::Front;
        case JSON_ShaderCullMode::back: return backend::CullMode::Back;
        case JSON_ShaderCullMode::none: return backend::CullMode::None;
        default: {
            assert(false && "invalid data type");
            return backend::CullMode::None;
        }
    }
}
