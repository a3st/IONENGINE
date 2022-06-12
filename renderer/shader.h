// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <lib/expected.h>
#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class JSON_ShaderUniformType {
    cbuffer,
    sbuffer,
    sampler2D,
    rwbuffer,
    rwtexture2D
};

JSON5_ENUM(JSON_ShaderUniformType, cbuffer, sbuffer, rwbuffer, sampler2D, rwtexture2D)

enum class JSON_ShaderDataType {
    float4x4,
    float4,
    float3,
    float2,
    _float,
    _uint,
    _bool
};

JSON5_ENUM(JSON_ShaderDataType, float4x4, float4, float3, float2, _float, _uint, _bool)

enum class JSON_ShaderType {
    vertex,
    geometry,
    hull,
    domain,
    pixel,
    compute,
    all
};

JSON5_ENUM(JSON_ShaderType, vertex, geometry, hull, domain, pixel, compute, all)

enum class JSON_ShaderFillMode {
    wireframe,
    solid
};

JSON5_ENUM(JSON_ShaderFillMode, wireframe, solid)

enum class JSON_ShaderCullMode {
    front,
    back,
    none
};

JSON5_ENUM(JSON_ShaderCullMode, front, back, none)

struct JSON_ShaderStructDefinition {
    std::string name;
    JSON_ShaderDataType type;
    std::optional<std::string> semantic;
};

JSON5_CLASS(JSON_ShaderStructDefinition, name, type, semantic)

struct JSON_ShaderUniformDefinition {
    std::string name;
    JSON_ShaderUniformType type;
    std::optional<std::vector<JSON_ShaderStructDefinition>> properties;
    JSON_ShaderType visibility;
};

JSON5_CLASS(JSON_ShaderUniformDefinition, name, type, properties, visibility)

struct JSON_ShaderStageDefinition {
    JSON_ShaderType type;
    std::vector<JSON_ShaderStructDefinition> inputs;
    std::vector<JSON_ShaderStructDefinition> outputs;
    std::string source;
};

JSON5_CLASS(JSON_ShaderStageDefinition, type, inputs, outputs, source)

struct JSON_ShaderDrawParametersDefinition {
    JSON_ShaderFillMode fill_mode;
    JSON_ShaderCullMode cull_mode;
    bool depth_stencil;
    bool blend;
};

JSON5_CLASS(JSON_ShaderDrawParametersDefinition, fill_mode, cull_mode, depth_stencil, blend)

struct JSON_ShaderDefinition {
    std::string name;
    std::vector<JSON_ShaderUniformDefinition> uniforms;
    JSON_ShaderDrawParametersDefinition draw_parameters;
    std::vector<JSON_ShaderStageDefinition> stages;
};

JSON5_CLASS(JSON_ShaderDefinition, name, uniforms, draw_parameters, stages)

namespace ionengine::renderer {

enum class ShaderError { 
    IO,
    ParseError
};

enum class ShaderUniformType {
    CBuffer,
    Sampler2D,
    SBuffer,
    RWBuffer,
    RWTexture2D
};

template<ShaderUniformType Type>
struct ShaderUniformData { };

template<>
struct ShaderUniformData<ShaderUniformType::Sampler2D> { 
    uint32_t index;
};

template<>
struct ShaderUniformData<ShaderUniformType::RWTexture2D> { 
    uint32_t index;
};

template<>
struct ShaderUniformData<ShaderUniformType::CBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

template<>
struct ShaderUniformData<ShaderUniformType::SBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

template<>
struct ShaderUniformData<ShaderUniformType::RWBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

#ifndef DECLARE_SHADER_UNIFORM_CAST
#define DECLARE_SHADER_UNIFORM_CAST(Name, Type) \
ShaderUniformData<Type> const& Name() const { \
    return std::get<ShaderUniformData<Type>>(data); \
}
#endif

struct ShaderUniform {
    std::variant<
        ShaderUniformData<ShaderUniformType::Sampler2D>,
        ShaderUniformData<ShaderUniformType::CBuffer>,
        ShaderUniformData<ShaderUniformType::SBuffer>,
        ShaderUniformData<ShaderUniformType::RWBuffer>,
        ShaderUniformData<ShaderUniformType::RWTexture2D>
    > data;

    DECLARE_SHADER_UNIFORM_CAST(as_sampler2D, ShaderUniformType::Sampler2D)
    DECLARE_SHADER_UNIFORM_CAST(as_cbuffer, ShaderUniformType::CBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_sbuffer, ShaderUniformType::SBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_rwbuffer, ShaderUniformType::RWBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_rwtexture2D, ShaderUniformType::RWTexture2D)
};

struct Shader {
    std::string name;
    std::vector<backend::Handle<backend::Shader>> stages;
    backend::Handle<backend::DescriptorLayout> descriptor_layout;
    std::vector<backend::VertexInputDesc> attributes;
    std::unordered_map<std::string, ShaderUniform> uniforms;

    backend::RasterizerDesc rasterizer;
    backend::DepthStencilDesc depth_stencil;
    backend::BlendDesc blend;

    uint64_t hash;

    uint32_t location_uniform_by_name(std::string_view const name) const;

    static lib::Expected<Shader, lib::Result<ShaderError>> load_from_file(std::filesystem::path const& shader_path, backend::Device& device);
};

template<>
struct GPUResourceDeleter<Shader> {
    void operator()(backend::Device& device, Shader const& shader) const {
        for(auto const& stage : shader.stages) {
            device.delete_shader(stage);
        }
    }
};

backend::ShaderFlags constexpr get_shader_flags(JSON_ShaderType const shader_type);

backend::Format constexpr get_attribute_format(JSON_ShaderDataType const data_type);

uint64_t constexpr get_shader_data_type_size(JSON_ShaderDataType const data_type);

std::string constexpr get_shader_data_type_string(JSON_ShaderDataType const data_type);

backend::FillMode constexpr get_fill_mode(JSON_ShaderFillMode const fill_mode);

backend::CullMode constexpr get_cull_mode(JSON_ShaderCullMode const cull_mode);

std::string generate_uniform_code(
    std::string_view const name, 
    JSON_ShaderUniformType const uniform_type, 
    uint32_t const location,
    std::optional<std::span<JSON_ShaderStructDefinition const>> const properties = std::nullopt
);

std::string generate_struct_code(
    std::string_view const name, 
    std::span<JSON_ShaderStructDefinition const> const properties
);

void calculate_uniform_offsets(
    std::span<JSON_ShaderStructDefinition const> const properties,
    std::unordered_map<std::string, uint64_t>& offsets
);

}
