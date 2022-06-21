// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/shader.h>
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::asset;

lib::Expected<Shader, lib::Result<ShaderError>> Shader::load_from_file(std::filesystem::path const& file_path) {

    std::string const file_path_string = file_path.string();

    auto document = JSON_ShaderDefinition {};
    json5::error const result = json5::from_file(file_path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::make_expected<Shader, lib::Result<ShaderError>>(
            lib::Result<ShaderError> { .errc = ShaderError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::make_expected<Shader, lib::Result<ShaderError>>(
            lib::Result<ShaderError> { .errc = ShaderError::ParseError, .message = std::format("Parse file error '{}'", file_path_string) }
        );
    }

    auto shader = Shader {};
    shader.name = document.name;

    for(auto const& uniform : document.uniforms) {

        ShaderUniform shader_uniform;

        switch(uniform.type) {
            case JSON_ShaderUniformType::cbuffer: {
                std::vector<ShaderBufferData> buffer_data;

                for(auto const& property : uniform.properties.value()) {
                    buffer_data.emplace_back(property.name, get_shader_data_type(property.type));
                }

                shader_uniform = ShaderUniform {
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::CBuffer>{ .data = std::move(buffer_data) },
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;
            case JSON_ShaderUniformType::rwbuffer: {
                std::vector<ShaderBufferData> buffer_data;

                for(auto const& property : uniform.properties.value()) {
                    buffer_data.emplace_back(property.name, get_shader_data_type(property.type));
                }

                shader_uniform = ShaderUniform {
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::RWBuffer>{ .data = std::move(buffer_data) },
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;
            case JSON_ShaderUniformType::sbuffer: {
                std::vector<ShaderBufferData> buffer_data;

                for(auto const& property : uniform.properties.value()) {
                    buffer_data.emplace_back(property.name, get_shader_data_type(property.type));
                }

                shader_uniform = ShaderUniform {
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::SBuffer>{ .data = std::move(buffer_data) },
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;
            case JSON_ShaderUniformType::rwtexture2D: {
                shader_uniform = ShaderUniform { 
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::RWTexture2D>{},
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;
            case JSON_ShaderUniformType::sampler2D: {
                shader_uniform = ShaderUniform { 
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::Sampler2D>{},
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;
        }

        shader.uniforms.emplace_back(std::move(shader_uniform));
    }

    std::vector<uint64_t> stage_hashes;

    for(auto const& stage : document.stages) {

        std::string generated_code = "";

        switch(stage.type) {

            case JSON_ShaderType::vertex: {
                generated_code += generate_shader_iassembler_code("vs_input", stage.inputs);
                generated_code += generate_shader_iassembler_code("vs_output", stage.outputs);

                for(auto const& attribute : stage.inputs) {
                    std::string const indices = "0123456789";
                    std::string semantic_only = attribute.semantic;

                    for(auto const& index : indices) {
                        if(attribute.semantic.back() == index) {
                            semantic_only.pop_back();
                            break;
                        }
                    }
                    shader.attributes.emplace_back(semantic_only, get_shader_data_type(attribute.type));
                }
            } break;

            case JSON_ShaderType::domain: {
                generated_code += generate_shader_iassembler_code("dom_input", stage.inputs);
                generated_code += generate_shader_iassembler_code("dom_output", stage.outputs);
            } break;

            case JSON_ShaderType::hull: {
                generated_code += generate_shader_iassembler_code("hull_input", stage.inputs);
                generated_code += generate_shader_iassembler_code("hull_output", stage.outputs);
            } break;

            case JSON_ShaderType::geometry: {
                generated_code += generate_shader_iassembler_code("geom_input", stage.inputs);
                generated_code += generate_shader_iassembler_code("geom_output", stage.outputs);
            } break;

            case JSON_ShaderType::pixel: {
                generated_code += generate_shader_iassembler_code("ps_input", stage.inputs);
                generated_code += generate_shader_iassembler_code("ps_output", stage.outputs);
            } break;
        }

        generated_code += stage.source;

        uint64_t const hash = XXHash64::hash(generated_code.data(), generated_code.size(), 0);

        auto stage_source = StageSource {
            .source = generated_code,
            .hash = hash
        };

        stage_hashes.emplace_back(std::move(hash));

        shader.stages.insert({ get_shader_type(stage.type), std::move(stage_source) });
    }

    shader.hash = std::accumulate(stage_hashes.begin(), stage_hashes.end(), stage_hashes.at(0), std::bit_xor<uint64_t>());

    shader.draw_parameters = ShaderDrawParameters {
        .fill_mode = get_shader_fill_mode(document.draw_parameters.fill_mode),
        .cull_mode = get_shader_cull_mode(document.draw_parameters.cull_mode),
        .depth_stencil = document.draw_parameters.depth_stencil,
        .blend_mode = get_shader_blend_mode(document.draw_parameters.blend_mode)
    };

    return lib::make_expected<Shader, lib::Result<ShaderError>>(std::move(shader));
}

std::string constexpr ionengine::asset::get_shader_data_type_string(JSON_ShaderDataType const data_type) {
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

ShaderDataType constexpr ionengine::asset::get_shader_data_type(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::float4x4: return ShaderDataType::Float4x4;
        case JSON_ShaderDataType::float4: return ShaderDataType::Float4;
        case JSON_ShaderDataType::float3: return ShaderDataType::Float3;
        case JSON_ShaderDataType::float2: return ShaderDataType::Float2;
        case JSON_ShaderDataType::_float: return ShaderDataType::Float;
        case JSON_ShaderDataType::_uint: return ShaderDataType::Uint;
        case JSON_ShaderDataType::_bool: return ShaderDataType::Bool;
        default: {
            assert(false && "invalid data type");
            return ShaderDataType::Float;
        }
    }
}

ShaderType constexpr ionengine::asset::get_shader_type(JSON_ShaderType const shader_type) {
    switch(shader_type) {
        case JSON_ShaderType::vertex: return ShaderType::Vertex;
        case JSON_ShaderType::pixel: return ShaderType::Pixel;
        case JSON_ShaderType::geometry: return ShaderType::Geometry;
        case JSON_ShaderType::domain: return ShaderType::Domain;
        case JSON_ShaderType::hull: return ShaderType::Hull;
        case JSON_ShaderType::compute: return ShaderType::Compute;
        case JSON_ShaderType::all: return ShaderType::All;
        default: {
            assert(false && "invalid data type");
            return ShaderType::All;
        }
    }
}

ShaderFillMode constexpr ionengine::asset::get_shader_fill_mode(JSON_ShaderFillMode const fill_mode) {
    switch(fill_mode) {
        case JSON_ShaderFillMode::solid: return ShaderFillMode::Solid;
        case JSON_ShaderFillMode::wireframe: return ShaderFillMode::Wireframe;
        default: {
            assert(false && "invalid data type");
            return ShaderFillMode::Solid;
        } 
    }
}

ShaderCullMode constexpr ionengine::asset::get_shader_cull_mode(JSON_ShaderCullMode const cull_mode) {
    switch(cull_mode) {
        case JSON_ShaderCullMode::front: return ShaderCullMode::Front;
        case JSON_ShaderCullMode::back: return ShaderCullMode::Back;
        case JSON_ShaderCullMode::none: return ShaderCullMode::None;
        default: {
            assert(false && "invalid data type");
            return ShaderCullMode::None;
        }
    }
}

ShaderBlendMode constexpr ionengine::asset::get_shader_blend_mode(JSON_ShaderBlendMode const blend_mode) {
    switch(blend_mode) {
        case JSON_ShaderBlendMode::opaque: return ShaderBlendMode::Opaque;
        case JSON_ShaderBlendMode::add: return ShaderBlendMode::Add;
        case JSON_ShaderBlendMode::mixed: return ShaderBlendMode::Mixed;
        case JSON_ShaderBlendMode::alpha_blend: return ShaderBlendMode::AlphaBlend;
        default: {
            assert(false && "invalid data type");
            return ShaderBlendMode::Opaque;
        }
    }
}

std::string ionengine::asset::generate_shader_iassembler_code(std::string_view const name, std::span<JSON_ShaderIAssemblerDefinition const> const properties) {
    std::string generated_code = "struct";
    generated_code += std::format(" {} {{ ", name);

    for(auto& property : properties) {
        generated_code += std::format("{} {} : {}; ", get_shader_data_type_string(property.type), property.name, property.semantic);
    }

    generated_code += "};\n";
    return generated_code;
}
