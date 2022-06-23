// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/shader.h>
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::asset;

std::vector<ShaderUniform> ionengine::asset::fill_shader_variant_uniforms(std::string_view const condition, std::span<JSON_ShaderUniformDefinition const> const data) {

    std::vector<ShaderUniform> uniforms;

    for(auto const& uniform : data) {

        if(uniform.condition.has_value()) {
            if(!uniform.condition->contains(condition)) {
                continue;
            }
        }

        ShaderUniform shader_uniform;

        switch(uniform.type) {
            
            case JSON_ShaderUniformType::cbuffer: {
                std::vector<ShaderBufferData> buffer_data;

                for(auto const& variable : uniform.data.value()) {
                    buffer_data.emplace_back(variable.name, get_shader_data_type(variable.type));
                }

                shader_uniform = ShaderUniform {
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::CBuffer>{ .data = std::move(buffer_data) },
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;

            case JSON_ShaderUniformType::rwbuffer: {
                std::vector<ShaderBufferData> buffer_data;

                for(auto const& variable : uniform.data.value()) {
                    buffer_data.emplace_back(variable.name, get_shader_data_type(variable.type));
                }

                shader_uniform = ShaderUniform {
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::RWBuffer>{ .data = std::move(buffer_data) },
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;

            case JSON_ShaderUniformType::sbuffer: {
                std::vector<ShaderBufferData> buffer_data;

                for(auto const& variable : uniform.data.value()) {
                    buffer_data.emplace_back(variable.name, get_shader_data_type(variable.type));
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

            case JSON_ShaderUniformType::samplerCube: {
                shader_uniform = ShaderUniform { 
                    .name = uniform.name,
                    .data = ShaderUniformData<ShaderUniformType::SamplerCube>{},
                    .visibility = get_shader_type(uniform.visibility)
                };
            } break;
        }

        uniforms.emplace_back(std::move(shader_uniform));
    }

    return std::move(uniforms);
}

std::unordered_map<ShaderType, StageSource> ionengine::asset::fill_shader_variant_stages(std::string_view const condition, std::span<JSON_ShaderStageDefinition const> const data) {

    std::unordered_map<ShaderType, StageSource> stages;

    for(auto const& stage : data) {
        std::string generated_code = "";

        generated_code += generate_shader_generic_code();
        generated_code += generate_shader_input_code(condition, stage.type, stage.inputs);
        generated_code += generate_shader_output_code(condition, stage.type, stage.outputs);
        generated_code += stage.source;

        uint64_t const hash = XXHash64::hash(generated_code.data(), generated_code.size(), 0);

        auto stage_source = StageSource {
            .source = generated_code,
            .hash = hash
        };

        stages.insert({ get_shader_type(stage.type), std::move(stage_source) });
    }

    return std::move(stages);
}

std::vector<VertexAttributeData> ionengine::asset::fill_shader_variant_attributes(std::string_view const condition, std::span<JSON_ShaderStageDefinition const> const data) {

    std::vector<VertexAttributeData> attributes;

    for(auto const& stage : data) {

        if(stage.type != JSON_ShaderType::vertex) {
            continue;
        }

        for(auto const& attribute : stage.inputs) {
            std::string const indices = "0123456789";
            std::string semantic_only = attribute.semantic;

            for(auto const& index : indices) {
                if(attribute.semantic.back() == index) {
                    semantic_only.pop_back();
                    break;
                }
            }
            attributes.emplace_back(semantic_only, get_shader_data_type(attribute.type));
        }
        break;
    }

    return std::move(attributes);
}

lib::Expected<Shader, lib::Result<ShaderError>> Shader::load_from_file(std::filesystem::path const& file_path) {

    std::string const file_path_string = file_path.string();

    JSON_ShaderDefinition document;
    json5::error const result = json5::from_file(file_path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::make_expected<Shader, lib::Result<ShaderError>>(
            lib::Result<ShaderError> { .errc = ShaderError::IO, .message = std::format("Could not open a file '{}'", file_path_string) }
        );
    }

    if(result != json5::error::none) {
        return lib::make_expected<Shader, lib::Result<ShaderError>>(
            lib::Result<ShaderError> { .errc = ShaderError::ParseError, .message = std::format("Parse file error '{}'", file_path_string) }
        );
    }

    std::unordered_map<uint16_t, ShaderVariant> variants;

    // Default ShaderVariant
    {
        std::vector<ShaderUniform> uniforms = fill_shader_variant_uniforms("", document.uniforms);
        std::unordered_map<ShaderType, StageSource> stages = fill_shader_variant_stages("", document.stages);
        std::vector<VertexAttributeData> attributes = fill_shader_variant_attributes("", document.stages);

        auto hashes = 
            stages |
            std::views::transform([&](std::pair<ShaderType, StageSource> const& other) { return other.second.hash; }) | 
            ranges::to<std::vector>();

        auto shader_variant = ShaderVariant {
            .uniforms = std::move(uniforms),
            .attributes = std::move(attributes),
            .stages = std::move(stages),
            .hash = std::accumulate(hashes.begin(), hashes.end(), hashes.at(0), std::bit_xor<uint64_t>()),
            .cache_entry = std::numeric_limits<size_t>::max()
        };

        variants.insert({ 1 << 0, std::move(shader_variant) });
    }

    std::unordered_map<std::string, uint16_t> conditions;

    uint16_t condition_mask = 1;

    for(auto const& condition : document.conditions) {
        conditions.insert({ condition, 1 << condition_mask });
        ++condition_mask;
    }

    auto draw_parameters = ShaderDrawParameters {
        .fill_mode = get_shader_fill_mode(document.draw_parameters.fill_mode),
        .cull_mode = get_shader_cull_mode(document.draw_parameters.cull_mode),
        .depth_test = get_shader_depth_test(document.draw_parameters.depth_test),
        .blend_mode = get_shader_blend_mode(document.draw_parameters.blend_mode),
    };

    auto shader = Shader {
        .name = document.name,
        .draw_parameters = std::move(draw_parameters),
        .variants = std::move(variants),
        .conditions = std::move(conditions)
    };

    return lib::make_expected<Shader, lib::Result<ShaderError>>(std::move(shader));
}

std::string constexpr ionengine::asset::get_shader_data_type_string(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::f32x4x4: return "float4x4";
        case JSON_ShaderDataType::f32x4: return "float4";
        case JSON_ShaderDataType::f32x3: return "float3";
        case JSON_ShaderDataType::f32x2: return "float2";
        case JSON_ShaderDataType::f32: return "float";
        case JSON_ShaderDataType::uint32: return "uint";
        case JSON_ShaderDataType::boolean: return "bool";
        default: {
            assert(false && "invalid data type");
            return "float";
        }
    }
}

ShaderDataType constexpr ionengine::asset::get_shader_data_type(JSON_ShaderDataType const data_type) {
    switch(data_type) {
        case JSON_ShaderDataType::f32x4x4: return ShaderDataType::F32x4x4;
        case JSON_ShaderDataType::f32x4: return ShaderDataType::F32x4;
        case JSON_ShaderDataType::f32x3: return ShaderDataType::F32x3;
        case JSON_ShaderDataType::f32x2: return ShaderDataType::F32x2;
        case JSON_ShaderDataType::f32: return ShaderDataType::F32;
        case JSON_ShaderDataType::uint32: return ShaderDataType::UInt32;
        case JSON_ShaderDataType::boolean: return ShaderDataType::Boolean;
        default: {
            assert(false && "invalid data type");
            return ShaderDataType::F32;
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

ShaderDepthTest constexpr ionengine::asset::get_shader_depth_test(JSON_ShaderDepthTest const depth_test) {
    switch(depth_test) {
        case JSON_ShaderDepthTest::always: return ShaderDepthTest::Always;
        case JSON_ShaderDepthTest::less: return ShaderDepthTest::Less;
        case JSON_ShaderDepthTest::less_equal: return ShaderDepthTest::LessEqual;
        case JSON_ShaderDepthTest::equal: return ShaderDepthTest::Equal;
        case JSON_ShaderDepthTest::none: return ShaderDepthTest::None;
        default: {
            assert(false && "invalid data type");
            return ShaderDepthTest::Always;
        }
    }
}

std::string ionengine::asset::generate_shader_input_code(std::string_view const condition, JSON_ShaderType const shader_type, std::span<JSON_ShaderInputOutputDefinition const> const data) {
    auto get_input_name = [](JSON_ShaderType const shader_type) -> std::string {
        switch(shader_type) {
            case JSON_ShaderType::vertex: return "vs_input";
            case JSON_ShaderType::domain: return "ds_input";
            case JSON_ShaderType::hull: return "hs_input";
            case JSON_ShaderType::geometry: return "gs_input";
            case JSON_ShaderType::pixel: return "ps_input";
            default: {
                assert(false && "invalid data type");
                return "";
            }
        }
    };
    
    std::string generated_code = "struct";

    generated_code += std::format(" {} {{ ", get_input_name(shader_type));

    for(auto& attribute : data) {
        if(attribute.condition.has_value()) {
            if(!attribute.condition->contains(condition)) {
                continue;
            }
        }
        generated_code += std::format("{} {} : {}; ", get_shader_data_type_string(attribute.type), attribute.name, attribute.semantic);
    }

    generated_code += "};\n";
    return generated_code;
}

std::string ionengine::asset::generate_shader_output_code(std::string_view const condition, JSON_ShaderType const shader_type, std::span<JSON_ShaderInputOutputDefinition const> const data) {
    auto get_input_name = [](JSON_ShaderType const shader_type) -> std::string {
        switch(shader_type) {
            case JSON_ShaderType::vertex: return "vs_output";
            case JSON_ShaderType::domain: return "ds_output";
            case JSON_ShaderType::hull: return "hs_output";
            case JSON_ShaderType::geometry: return "gs_output";
            case JSON_ShaderType::pixel: return "ps_output";
            default: {
                assert(false && "invalid data type");
                return "";
            }
        }
    };
    
    std::string generated_code = "struct";

    generated_code += std::format(" {} {{ ", get_input_name(shader_type));

    for(auto& attribute : data) {
        if(attribute.condition.has_value()) {
            if(!attribute.condition->contains(condition)) {
                continue;
            }
        }
        generated_code += std::format("{} {} : {}; ", get_shader_data_type_string(attribute.type), attribute.name, attribute.semantic);
    }

    generated_code += "};\n";
    return generated_code;
}

std::string ionengine::asset::generate_shader_generic_code() {
    std::string generated_code = "";

    // Types
    generated_code += "#define f32 float \n";
    generated_code += "#define f32x2 float2 \n";
    generated_code += "#define f32x3 float3 \n";
    generated_code += "#define f32x4 float4 \n";
    generated_code += "#define f32x4x4 float4x4 \n";
    generated_code += "#define f32x3x3 float3x3 \n";
    generated_code += "#define uint32 uint \n";
    generated_code += "#define boolean bool \n";

    // Functions
    generated_code += "#define TEXTURE_SAMPLE(Texture, UV) Texture##_texture.Sample(Texture##_sampler, UV) \n";

    return generated_code;
}