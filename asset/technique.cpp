// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/technique.h>
#include <lib/exception.h>

using namespace ionengine::asset;

Technique::Technique(std::filesystem::path const& file_path) {

    std::string from_path_string = file_path.string();

    json5::error result = json5::from_file(from_path_string, _data);

    if(result != json5::error::none) {
        throw lib::Exception(std::format("File load {}!", from_path_string));
    }

    for(auto& shader : _data.shaders) {

        std::string shader_code = "";

        switch(shader.type) {
            case ShaderType::vertex: {
                shader_code += generate_vertex_assembler_code("vs_input", shader.inputs);
                shader_code += generate_vertex_assembler_code("vs_output", shader.outputs);
            } break;
            case ShaderType::pixel: {
                shader_code += generate_vertex_assembler_code("ps_input", shader.inputs);
                shader_code += generate_vertex_assembler_code("ps_output", shader.outputs);
            } break;
        }

        for(auto& property : _data.properties) {

            if(property.visibility != shader.type) {
                continue;
            }

            if(property.properties.has_value()) {
                shader_code += generate_uniform_code(property.name, property.type, property.location, property.properties.value());
            } else {
                shader_code += generate_uniform_code(property.name, property.type, property.location);
            }
        }

        shader_code += shader.source;

        shader.source = shader_code;
    }
}

TechniqueData const& Technique::data() const {

    return _data;
}

std::string Technique::generate_uniform_code(
    std::string_view const name, 
    ShaderUniformType const uniform_type, 
    uint32_t const location,
    std::optional<std::span<ShaderUniformPropertyDefinition const>> const properties
) {

    if(uniform_type == ShaderUniformType::cbuffer) {

        std::string generated_code = "cbuffer";
        generated_code += std::format(" {} : register(b{}) {{ ", name, location);
        for(auto& property : properties.value()) {
            generated_code += std::format("{} {}; ", get_shader_data_type(property.type), property.name);
        }
        generated_code += "};\n";
        return generated_code;

    } else if(uniform_type == ShaderUniformType::sampler2D) {

        std::string generated_code = "SamplerState";
        generated_code += std::format(" {}_sampler : register(s{}); ", name, location);
        generated_code += "Texture2D";
        generated_code += std::format(" {}_texture : register(t{}); ", name, location);
        generated_code += "\n";
        return generated_code;
    }

    return "";
}

std::string Technique::generate_vertex_assembler_code(
    std::string_view const name, 
    std::span<VertexAssemblerDefinition const> const properties
) {

    std::string generated_code = "struct";
    generated_code += std::format(" {} {{ ", name);
    for(auto& property : properties) {
        generated_code += std::format("{} {} : {}; ", get_shader_data_type(property.type), property.name, property.semantic);
    }
    generated_code += "};\n";
    return generated_code;
}

std::string constexpr Technique::get_shader_data_type(ShaderDataType const data_type) {

    switch(data_type) {
        case ShaderDataType::f32x4x4: return "float4x4";
        case ShaderDataType::f32x4: return "float4";
        case ShaderDataType::f32x3: return "float3";
        case ShaderDataType::f32x2: return "float2";
        case ShaderDataType::f32: return "float";
        default: return "float";
    }
}
