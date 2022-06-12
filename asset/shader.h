// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

struct JSON_ShaderBufferDataDefinition {
    std::string name;
    JSON_ShaderDataType type;
};

JSON5_CLASS(JSON_ShaderBufferDataDefinition, name, type)

struct JSON_ShaderIAssemblerDefinition {
    std::string name;
    JSON_ShaderDataType type;
    std::string semantic;
};

JSON5_CLASS(JSON_ShaderIAssemblerDefinition, name, type, semantic)

struct JSON_ShaderUniformDefinition {
    std::string name;
    JSON_ShaderUniformType type;
    std::optional<std::vector<JSON_ShaderBufferDataDefinition>> properties;
    JSON_ShaderType visibility;
};

JSON5_CLASS(JSON_ShaderUniformDefinition, name, type, properties, visibility)

struct JSON_ShaderStageDefinition {
    JSON_ShaderType type;
    std::vector<JSON_ShaderIAssemblerDefinition> inputs;
    std::vector<JSON_ShaderIAssemblerDefinition> outputs;
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

namespace ionengine::asset {

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

enum class ShaderDataType {
    Float4x4,
    Float4,
    Float3,
    Float2,
    Float,
    Uint,
    Bool
};

enum class ShaderType {
    Vertex,
    Geometry,
    Hull,
    Domain,
    Pixel,
    Compute,
    All
};

enum class ShaderFillMode {
    Wireframe,
    Solid
};

enum class ShaderCullMode {
    Front,
    Back,
    None
};

struct ShaderBufferData {
    std::string name;
    ShaderDataType type;
};

template<ShaderUniformType Type>
struct ShaderUniformData { };

template<>
struct ShaderUniformData<ShaderUniformType::Sampler2D> { };

template<>
struct ShaderUniformData<ShaderUniformType::RWTexture2D> { };

template<>
struct ShaderUniformData<ShaderUniformType::CBuffer> {
    std::vector<ShaderBufferData> data;
};

template<>
struct ShaderUniformData<ShaderUniformType::SBuffer> {
    std::vector<ShaderBufferData> data;
};

template<>
struct ShaderUniformData<ShaderUniformType::RWBuffer> {
    std::vector<ShaderBufferData> data;
};

#ifndef DECLARE_SHADER_UNIFORM_CONST_CAST
#define DECLARE_SHADER_UNIFORM_CONST_CAST(Name, Type) \
ShaderUniformData<Type> const& Name() const { \
    return std::get<ShaderUniformData<Type>>(data); \
}
#endif

struct ShaderUniform {
    std::string name;

    std::variant<
        ShaderUniformData<ShaderUniformType::Sampler2D>,
        ShaderUniformData<ShaderUniformType::RWTexture2D>,
        ShaderUniformData<ShaderUniformType::CBuffer>,
        ShaderUniformData<ShaderUniformType::SBuffer>,
        ShaderUniformData<ShaderUniformType::RWBuffer>
    > data;

    ShaderType visibility;

    DECLARE_SHADER_UNIFORM_CONST_CAST(as_sampler2D, ShaderUniformType::Sampler2D)
    DECLARE_SHADER_UNIFORM_CONST_CAST(as_cbuffer, ShaderUniformType::CBuffer)
    DECLARE_SHADER_UNIFORM_CONST_CAST(as_sbuffer, ShaderUniformType::SBuffer)
    DECLARE_SHADER_UNIFORM_CONST_CAST(as_rwbuffer, ShaderUniformType::RWBuffer)
    DECLARE_SHADER_UNIFORM_CONST_CAST(as_rwtexture2D, ShaderUniformType::RWTexture2D)

    bool is_buffer() const {
        return data.index() > 1;
    }
};

struct StageSource {
    std::string source;
    uint64_t hash;
};

struct VertexAttributeData {
    std::string semantic;
    ShaderDataType type;
};

struct ShaderDrawParameters {
    ShaderFillMode fill_mode;
    ShaderCullMode cull_mode;
    bool depth_stencil;
    bool blend;
};

struct Shader {
    std::string name;
    std::vector<ShaderUniform> uniforms;
    std::unordered_map<ShaderType, StageSource> stages;
    std::vector<VertexAttributeData> attributes;
    ShaderDrawParameters draw_parameters;
    uint64_t hash;

    size_t cache_entry;

    static lib::Expected<Shader, lib::Result<ShaderError>> load_from_file(std::filesystem::path const& file_path); 
};

std::string constexpr get_shader_data_type_string(JSON_ShaderDataType const data_type);

ShaderDataType constexpr get_shader_data_type(JSON_ShaderDataType const data_type);

ShaderType constexpr get_shader_type(JSON_ShaderType const shader_type);

ShaderFillMode constexpr get_shader_fill_mode(JSON_ShaderFillMode const fill_mode);

ShaderCullMode constexpr get_shader_cull_mode(JSON_ShaderCullMode const cull_mode);

std::string generate_shader_iassembler_code(std::string_view const name, std::span<JSON_ShaderIAssemblerDefinition const> const properties);

}
