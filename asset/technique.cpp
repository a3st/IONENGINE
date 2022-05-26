// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/technique.h>
#include <lib/exception.h>
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::asset;

lib::Expected<Technique, lib::Result<TechniqueError>> Technique::load_from_file(std::filesystem::path const& file_path) {

    std::string path_string = file_path.string();

    JSON_TechniqueDefinition document;
    json5::error result = json5::from_file(path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::Expected<Technique, lib::Result<TechniqueError>>::error(
            lib::Result<TechniqueError> { .errc = TechniqueError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::Expected<Technique, lib::Result<TechniqueError>>::error(
            lib::Result<TechniqueError> { .errc = TechniqueError::ParseError, .message = "Parse file error" }
        );
    }

    auto technique = Technique {};
    {
        technique.name = document.name;

        std::unordered_map<std::string, uint32_t> locations;
        std::unordered_map<ShaderUniformType, uint32_t> registers_count;
        registers_count.insert({ ShaderUniformType::CBuffer, 0 });
        registers_count.insert({ ShaderUniformType::Sampler2D, 0 });
        registers_count.insert({ ShaderUniformType::RWTexture2D, 0 });

        for(auto const& uniform : document.uniforms) {

            technique.uniforms.emplace_back();
            technique.uniforms.back().name = uniform.name;

            if(uniform.properties.has_value() && uniform.type != JSON_ShaderUniformType::sampler2D) {

                if(uniform.type == JSON_ShaderUniformType::sbuffer) {

                    auto& data = technique.uniforms.back().data.emplace<ShaderUniformData<ShaderUniformType::SBuffer>>();

                    for(auto const& property : uniform.properties.value()) {
                        data.data.emplace_back(property.name, get_shader_data_type(property.type));
                    }

                    locations.insert({ uniform.name, registers_count.at(ShaderUniformType::Sampler2D) });
                    ++registers_count.at(ShaderUniformType::Sampler2D);

                } else if(uniform.type == JSON_ShaderUniformType::rwbuffer) {

                    auto& data = technique.uniforms.back().data.emplace<ShaderUniformData<ShaderUniformType::RWBuffer>>();

                    for(auto const& property : uniform.properties.value()) {
                        data.data.emplace_back(property.name, get_shader_data_type(property.type));
                    }

                    locations.insert({ uniform.name, registers_count.at(ShaderUniformType::RWTexture2D) });
                    ++registers_count.at(ShaderUniformType::RWTexture2D);

                } else {

                    auto& data = technique.uniforms.back().data.emplace<ShaderUniformData<ShaderUniformType::CBuffer>>();

                    for(auto const& property : uniform.properties.value()) {
                        data.data.emplace_back(property.name, get_shader_data_type(property.type));
                    }

                    locations.insert({ uniform.name, registers_count.at(ShaderUniformType::CBuffer) });
                    ++registers_count.at(ShaderUniformType::CBuffer);
                }
                
            } else {

                if(uniform.type == JSON_ShaderUniformType::rwtexture2D) {

                    technique.uniforms.back().data.emplace<ShaderUniformData<ShaderUniformType::RWTexture2D>>();

                    locations.insert({ uniform.name, registers_count.at(ShaderUniformType::RWTexture2D) });
                    ++registers_count.at(ShaderUniformType::RWTexture2D);

                } else {

                    technique.uniforms.back().data.emplace<ShaderUniformData<ShaderUniformType::Sampler2D>>();

                    locations.insert({ uniform.name, registers_count.at(ShaderUniformType::Sampler2D) });
                    ++registers_count.at(ShaderUniformType::Sampler2D);
                }
            }

            technique.uniforms.back().visibility = get_shader_flags(uniform.visibility.value_or(JSON_ShaderType::all));
        }

        std::vector<uint64_t> hashes;

        for(auto const& shader : document.shaders) {

            std::string shader_code = "";

            if(shader.imports.has_value()) {
                for(auto const& import : shader.imports.value()) {
                    generate_include_code(file_path.parent_path(), import);
                }
            }

            switch(shader.type) {
                case JSON_ShaderType::vertex: {
                    shader_code += generate_struct_code("vs_input", shader.inputs);
                    shader_code += generate_struct_code("vs_output", shader.outputs);

                    for(auto const& input : shader.inputs) {
                        technique.attributes.emplace_back(input.semantic.value(), get_shader_data_type(input.type));
                    }
                } break;
                case JSON_ShaderType::pixel: {
                    shader_code += generate_struct_code("ps_input", shader.inputs);
                    shader_code += generate_struct_code("ps_output", shader.outputs);
                } break;
                // TODO! Hull, Domain and Geometry
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
            uint64_t const hash = XXHash64::hash(reinterpret_cast<void*>(shader_code.data()), shader_code.size(), 0);

            hashes.emplace_back(hash);
            technique.shaders.emplace_back(shader_code, get_shader_flags(shader.type), hash);
        }

        // calculate total technique hash
        {
            technique.hash = hashes.at(0);
            for(size_t i = 1; i < hashes.size(); ++i) {
                technique.hash ^= hashes.at(i);
            }
        }
    }
    return lib::Expected<Technique, lib::Result<TechniqueError>>::ok(std::move(technique));
}

std::string ionengine::asset::generate_uniform_code(
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

std::string ionengine::asset::generate_struct_code(
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

std::string ionengine::asset::generate_include_code(std::filesystem::path const& directory, std::filesystem::path const& include_path) {
    std::filesystem::path const absolute_path = directory / include_path;
    return std::format("#include \"{}\"", absolute_path.string());
}

std::string constexpr ionengine::asset::get_shader_data_type_string(JSON_ShaderDataType const data_type) {

    switch(data_type) {
        case JSON_ShaderDataType::f32x4x4: return "float4x4";
        case JSON_ShaderDataType::f32x4: return "float4";
        case JSON_ShaderDataType::f32x3: return "float3";
        case JSON_ShaderDataType::f32x2: return "float2";
        case JSON_ShaderDataType::f32: return "float";
        case JSON_ShaderDataType::uint32: return "uint";
        case JSON_ShaderDataType::boolean: return "boolean";
        default: {
            assert(false && "invalid data type");
            return "float";
        }
    }
}

ShaderDataType constexpr ionengine::asset::get_shader_data_type(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::f32: return ShaderDataType::F32;
        case JSON_ShaderDataType::f32x2: return ShaderDataType::F32x2;
        case JSON_ShaderDataType::f32x3: return ShaderDataType::F32x3;
        case JSON_ShaderDataType::f32x4: return ShaderDataType::F32x4;
        case JSON_ShaderDataType::f32x4x4: return ShaderDataType::F32x4x4;
        case JSON_ShaderDataType::uint32: return ShaderDataType::UInt32;
        case JSON_ShaderDataType::boolean: return ShaderDataType::Boolean;
        default: {
            assert(false && "invalid data type");
            return ShaderDataType::F32;
        }
    }
}

ShaderFlags constexpr ionengine::asset::get_shader_flags(JSON_ShaderType const shader_type) {
    switch(shader_type) {
        case JSON_ShaderType::vertex: return ShaderFlags::Vertex;
        case JSON_ShaderType::geometry: return ShaderFlags::Geometry;
        case JSON_ShaderType::domain: return ShaderFlags::Domain;
        case JSON_ShaderType::hull: return ShaderFlags::Hull;
        case JSON_ShaderType::pixel: return ShaderFlags::Pixel;
        case JSON_ShaderType::compute: return ShaderFlags::Compute;
        case JSON_ShaderType::all: return ShaderFlags::All;
        default: {
            assert(false && "invalid data type");
            return ShaderFlags::All;
        }
    }
}
