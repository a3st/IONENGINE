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
    samplerCube,
    rwbuffer,
    rwtexture2D
};

JSON5_ENUM(JSON_ShaderUniformType, cbuffer, sbuffer, rwbuffer, sampler2D, samplerCube, rwtexture2D)

enum class JSON_ShaderDataType {
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32,
    uint32,
    boolean
};

JSON5_ENUM(JSON_ShaderDataType, f32x4x4, f32x4, f32x3, f32x2, f32, uint32, boolean)

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

enum class JSON_ShaderBlendMode {
    opaque,
    add,
    mixed,
    alpha_blend
};

JSON5_ENUM(JSON_ShaderBlendMode, opaque, add, mixed, alpha_blend)

enum class JSON_ShaderDepthTest {
    equal,
    less,
    less_equal,
    always,
    none
};

JSON5_ENUM(JSON_ShaderDepthTest, equal, less, less_equal, always, none)

struct JSON_ShaderBufferDataDefinition {
    std::string name;
    JSON_ShaderDataType type;
};

JSON5_CLASS(JSON_ShaderBufferDataDefinition, name, type)

struct JSON_ShaderInputOutputDefinition {
    std::string name;
    JSON_ShaderDataType type;
    std::string semantic;
    std::optional<std::string> condition;
};

JSON5_CLASS(JSON_ShaderInputOutputDefinition, name, type, semantic, condition)

struct JSON_ShaderUniformDefinition {
    std::string name;
    JSON_ShaderUniformType type;
    std::optional<std::vector<JSON_ShaderBufferDataDefinition>> data;
    JSON_ShaderType visibility;
    std::optional<std::string> condition;
};

JSON5_CLASS(JSON_ShaderUniformDefinition, name, type, data, visibility)

struct JSON_ShaderStageDefinition {
    JSON_ShaderType type;
    std::vector<JSON_ShaderInputOutputDefinition> inputs;
    std::vector<JSON_ShaderInputOutputDefinition> outputs;
    std::string source;
};

JSON5_CLASS(JSON_ShaderStageDefinition, type, inputs, outputs, source)

struct JSON_ShaderDrawParametersDefinition {
    JSON_ShaderFillMode fill_mode;
    JSON_ShaderCullMode cull_mode;
    JSON_ShaderDepthTest depth_test;
    JSON_ShaderBlendMode blend_mode;
};

JSON5_CLASS(JSON_ShaderDrawParametersDefinition, fill_mode, cull_mode, depth_test, blend_mode)

struct JSON_ShaderDefinition {
    std::string name;
    std::vector<std::string> conditions;
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
    SamplerCube,
    SBuffer,
    RWBuffer,
    RWTexture2D
};

enum class ShaderDataType {
    F32x4x4,
    F32x4,
    F32x3,
    F32x2,
    F32,
    UInt32,
    Boolean
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

enum class ShaderBlendMode {
    Opaque,
    Add,
    Mixed,
    AlphaBlend
};

enum class ShaderDepthTest {
    Equal,
    Less,
    LessEqual,
    Always,
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
struct ShaderUniformData<ShaderUniformType::SamplerCube> { };

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
        ShaderUniformData<ShaderUniformType::SamplerCube>,
        ShaderUniformData<ShaderUniformType::RWTexture2D>,
        ShaderUniformData<ShaderUniformType::CBuffer>,
        ShaderUniformData<ShaderUniformType::SBuffer>,
        ShaderUniformData<ShaderUniformType::RWBuffer>
    > data;

    ShaderType visibility;

    DECLARE_SHADER_UNIFORM_CONST_CAST(as_sampler2D, ShaderUniformType::Sampler2D)
    DECLARE_SHADER_UNIFORM_CONST_CAST(as_samplerCube, ShaderUniformType::SamplerCube)
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
    ShaderDepthTest depth_test;
    ShaderBlendMode blend_mode;
};

struct ShaderVariant {
    std::vector<ShaderUniform> uniforms;
    std::vector<VertexAttributeData> attributes;
    std::unordered_map<ShaderType, StageSource> stages;
    uint64_t hash;
    size_t cache_entry;
};

struct Shader {
    std::string name;
    ShaderDrawParameters draw_parameters;
    std::unordered_map<uint16_t, ShaderVariant> variants;
    std::unordered_map<std::string, uint16_t> conditions;

    static lib::Expected<Shader, lib::Result<ShaderError>> load_from_file(std::filesystem::path const& file_path); 
};

std::vector<ShaderUniform> fill_shader_variant_uniforms(std::string_view const condition, std::span<JSON_ShaderUniformDefinition const> const data);

std::unordered_map<ShaderType, StageSource> fill_shader_variant_stages(std::string_view const condition, std::span<JSON_ShaderStageDefinition const> const data);

std::vector<VertexAttributeData> fill_shader_variant_attributes(std::string_view const condition, std::span<JSON_ShaderStageDefinition const> const data);

std::string constexpr get_shader_data_type_string(JSON_ShaderDataType const data_type);

ShaderDataType constexpr get_shader_data_type(JSON_ShaderDataType const data_type);

ShaderType constexpr get_shader_type(JSON_ShaderType const shader_type);

ShaderFillMode constexpr get_shader_fill_mode(JSON_ShaderFillMode const fill_mode);

ShaderCullMode constexpr get_shader_cull_mode(JSON_ShaderCullMode const cull_mode);

ShaderBlendMode constexpr get_shader_blend_mode(JSON_ShaderBlendMode const blend_mode);

ShaderDepthTest constexpr get_shader_depth_test(JSON_ShaderDepthTest const depth_test);

std::string generate_shader_input_code(std::string_view const condition, JSON_ShaderType const shader_type, std::span<JSON_ShaderInputOutputDefinition const> const data);

std::string generate_shader_output_code(std::string_view const condition, JSON_ShaderType const shader_type, std::span<JSON_ShaderInputOutputDefinition const> const data);

std::string generate_shader_generic_code();

}
