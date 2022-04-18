// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/technique.h>
#include <lib/exception.h>

using namespace ionengine::asset;

Technique::Technique(std::filesystem::path const& file_path) {

    std::string from_path_string = file_path.string();

    JSON_TechniqueDefinition document;
    json5::error result = json5::from_file(from_path_string, document);

    if(result != json5::error::none) {
        throw lib::Exception(std::format("File load {}!", from_path_string));
    }

    _name = document.name;

    std::unordered_map<ShaderDataFlags, uint32_t> registers_count;
    registers_count.insert({ ShaderDataFlags::Constant, 0 });
    registers_count.insert({ ShaderDataFlags::CBuffer, 0 });
    registers_count.insert({ ShaderDataFlags::Sampler2D, 0 });

    std::unordered_map<std::string, uint32_t> locations;

    std::for_each(
        document.uniforms.begin(),
        document.uniforms.end(),
        [&](auto const& uniform) {

            _uniforms.emplace_back();
            _uniforms.back().name = uniform.name;

            if(uniform.properties.has_value() && uniform.type != JSON_ShaderDataType::sampler2D) {

                if(uniform.type != JSON_ShaderDataType::cbuffer) {

                    auto& data = _uniforms.back().data.emplace<ShaderUniformData<ShaderDataFlags::Constant>>();
                    data.flags = get_shader_data_flags(uniform.type);

                    locations.insert({ uniform.name, registers_count.at(ShaderDataFlags::Constant) });
                    ++registers_count.at(ShaderDataFlags::Constant);
                } else {

                    auto& data = _uniforms.back().data.emplace<ShaderUniformData<ShaderDataFlags::CBuffer>>();

                    std::for_each(
                        uniform.properties.value().begin(), 
                        uniform.properties.value().end(), 
                        [&](auto& element) {

                            data.data.emplace_back(element.name, get_shader_data_flags(element.type));
                        }
                    );

                    locations.insert({ uniform.name, registers_count.at(ShaderDataFlags::CBuffer) });
                    ++registers_count.at(ShaderDataFlags::CBuffer);
                }
            } else {

                _uniforms.back().data.emplace<ShaderUniformData<ShaderDataFlags::Sampler2D>>();

                locations.insert({ uniform.name, registers_count.at(ShaderDataFlags::Sampler2D) });
                ++registers_count.at(ShaderDataFlags::Sampler2D);
            }

            _uniforms.back().visibility = get_shader_flags(uniform.visibility.value_or(static_cast<JSON_ShaderType>(-1)));
        }
    );

    std::for_each(
        document.shaders.begin(),
        document.shaders.end(),
        [&](auto const& shader) {

            std::string shader_code = "";

            switch(shader.type) {
                case JSON_ShaderType::vertex: {
                    shader_code += generate_struct_code("vs_input", shader.inputs);
                    shader_code += generate_struct_code("vs_output", shader.outputs);
                } break;
                case JSON_ShaderType::pixel: {
                    shader_code += generate_struct_code("ps_input", shader.inputs);
                    shader_code += generate_struct_code("ps_output", shader.outputs);
                } break;
            }

            for(auto& uniform : document.uniforms) {

                if(uniform.visibility.has_value()) {
                    if(uniform.visibility.value() != shader.type) {
                        continue;
                    }
                }

                if(uniform.properties.has_value()) {
                    shader_code += generate_uniform_code(uniform.name, uniform.type, locations.at(uniform.name), uniform.properties.value());
                } else {
                    shader_code += generate_uniform_code(uniform.name, uniform.type, locations.at(uniform.name));
                }
            }

            shader_code += shader.source;

            _data.emplace_back(shader_code, get_shader_flags(shader.type));
        }
    );
}

std::string_view Technique::name() const {

    return _name;
}

std::span<ShaderUniform const> Technique::uniforms() const {

    return _uniforms;
}

std::span<ShaderData const> Technique::data() const {

    return _data;
}

std::string Technique::generate_uniform_code(
    std::string_view const name, 
    JSON_ShaderDataType const uniform_type, 
    uint32_t const location,
    std::optional<std::span<JSON_ShaderStructDefinition const>> const properties
) const {

    std::string generated_code = "";

    switch(uniform_type) {

        case JSON_ShaderDataType::cbuffer: {
            generated_code += std::format("cbuffer {} : register(b{}) {{ ", name, location);
            for(auto& property : properties.value()) {
                generated_code += std::format("{} {}; ", get_shader_data_string(property.type), property.name);
            }
            generated_code += "};\n";
        } break;

        case JSON_ShaderDataType::sampler2D: {
            generated_code += std::format("SamplerState {}_sampler : register(s{}); ", name, location);
            generated_code += std::format("Texture2D {}_texture : register(t{});\n", name, location);
        } break;

        default: {
            generated_code += std::format("{} {} : register(c{});\n", get_shader_data_string(uniform_type), name, location);
        } break;
    }

    return generated_code;
}

std::string Technique::generate_struct_code(
    std::string_view const name, 
    std::span<JSON_ShaderStructDefinition const> const properties
) const {

    std::string generated_code = "struct";
    generated_code += std::format(" {} {{ ", name);
    for(auto& property : properties) {
        generated_code += std::format("{} {} : {}; ", get_shader_data_string(property.type), property.name, property.semantic.value_or(""));
    }
    generated_code += "};\n";
    return generated_code;
}

std::string constexpr Technique::get_shader_data_string(JSON_ShaderDataType const data_type) const {

    switch(data_type) {
        case JSON_ShaderDataType::f32x4x4: return "float4x4";
        case JSON_ShaderDataType::f32x4: return "float4";
        case JSON_ShaderDataType::f32x3: return "float3";
        case JSON_ShaderDataType::f32x2: return "float2";
        case JSON_ShaderDataType::f32: return "float";
        default: return "float";
    }
}

ShaderDataFlags constexpr Technique::get_shader_data_flags(JSON_ShaderDataType const data_type) const {

    switch(data_type) {
        case JSON_ShaderDataType::f32: return ShaderDataFlags::F32;
        case JSON_ShaderDataType::f32x2: return ShaderDataFlags::F32x2;
        case JSON_ShaderDataType::f32x3: return ShaderDataFlags::F32x3;
        case JSON_ShaderDataType::f32x4: return ShaderDataFlags::F32x4;
        case JSON_ShaderDataType::f32x4x4: return ShaderDataFlags::F32x4x4;
        default: return ShaderDataFlags::F32;
    }
}

ShaderFlags constexpr Technique::get_shader_flags(JSON_ShaderType const shader_type) const {

    switch(shader_type) {
        case JSON_ShaderType::vertex: return ShaderFlags::Vertex;
        case JSON_ShaderType::geometry: return ShaderFlags::Geometry;
        case JSON_ShaderType::domain: return ShaderFlags::Domain;
        case JSON_ShaderType::hull: return ShaderFlags::Hull;
        case JSON_ShaderType::pixel: return ShaderFlags::Pixel;
        case JSON_ShaderType::compute: return ShaderFlags::Compute;
        default: return ShaderFlags::All;
    }
}
