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

enum class JSON_FillMode {
    wireframe,
    solid
};

JSON5_ENUM(JSON_FillMode, wireframe, solid)

enum class JSON_CullMode {
    front,
    back,
    none
};

JSON5_ENUM(JSON_CullMode, front, back, none)

struct JSON_ShaderStructDefinition {
    std::string name;
    JSON_ShaderDataType type;
    std::optional<std::string> semantic;
};

JSON5_CLASS(JSON_ShaderStructDefinition, name, type, semantic)

struct JSON_TechniqueUniformDefinition {
    std::string name;
    JSON_ShaderUniformType type;
    std::optional<std::vector<JSON_ShaderStructDefinition>> properties;
    std::optional<JSON_ShaderType> visibility;
};

JSON5_CLASS(JSON_TechniqueUniformDefinition, name, type, properties, visibility)

struct JSON_TechniqueShaderDefinition {
    JSON_ShaderType type;
    std::vector<JSON_ShaderStructDefinition> inputs;
    std::vector<JSON_ShaderStructDefinition> outputs;
    std::string source;
};

JSON5_CLASS(JSON_TechniqueShaderDefinition, type, inputs, outputs, source)

struct JSON_TechniqueDrawParametersDefinition {
    JSON_FillMode fill_mode;
    JSON_CullMode cull_mode;
    bool depth_stencil;
};

JSON5_CLASS(JSON_TechniqueDrawParametersDefinition, fill_mode, cull_mode, depth_stencil)

struct JSON_TechniqueDefinition {
    std::string name;
    std::vector<JSON_TechniqueUniformDefinition> uniforms;
    JSON_TechniqueDrawParametersDefinition draw_parameters;
    std::vector<JSON_TechniqueShaderDefinition> shaders;
};

JSON5_CLASS(JSON_TechniqueDefinition, name, uniforms, draw_parameters, shaders)

namespace ionengine::asset {

enum class TechniqueError {
    IO,
    ParseError
};

enum class ShaderFlags : uint16_t {
    Vertex = 1 << 0,
    Geometry = 1 << 1,
    Hull = 1 << 2,
    Domain = 1 << 3,
    Pixel = 1 << 4,
    Compute = 1 << 5,
    All = Vertex | Geometry | Hull | Domain | Pixel | Compute
};

DECLARE_ENUM_CLASS_BIT_FLAG(ShaderFlags)

enum class ShaderUniformType {
    CBuffer,
    SBuffer,
    Sampler2D,
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

enum class FillMode {
    Wireframe,
    Solid
};

enum class CullMode {
    Front,
    Back,
    None
};

template<ShaderUniformType Type>
struct ShaderUniformData { };

template<>
struct ShaderUniformData<ShaderUniformType::Sampler2D> { };

template<>
struct ShaderUniformData<ShaderUniformType::RWTexture2D> { };

struct ShaderBufferData {
    std::string name;
    ShaderDataType type;
};

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

#ifndef DECLARE_SHADER_UNIFORM_CAST
#define DECLARE_SHADER_UNIFORM_CAST(Name, Type) \
ShaderUniformData<Type> const& Name() { \
    return std::get<ShaderUniformData<Type>>(data); \
}
#endif

struct ShaderUniform {
    std::string name;

    std::variant<
        ShaderUniformData<ShaderUniformType::Sampler2D>,
        ShaderUniformData<ShaderUniformType::CBuffer>,
        ShaderUniformData<ShaderUniformType::SBuffer>,
        ShaderUniformData<ShaderUniformType::RWBuffer>,
        ShaderUniformData<ShaderUniformType::RWTexture2D>
    > data;

    ShaderFlags visibility;

    DECLARE_SHADER_UNIFORM_CAST(as_sampler2D, ShaderUniformType::Sampler2D)
    DECLARE_SHADER_UNIFORM_CAST(as_cbuffer, ShaderUniformType::CBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_sbuffer, ShaderUniformType::SBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_rwbuffer, ShaderUniformType::RWBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_rwtexture2D, ShaderUniformType::RWTexture2D)
};

struct VertexAttribute {
    std::string semantic;
    ShaderDataType type;
};

struct ShaderData {
    std::string source;
    ShaderFlags flags;
    uint64_t hash;
};

struct DrawParameters {
    FillMode fill_mode;
    CullMode cull_mode;
    bool depth_stencil;
};

struct Technique {
    std::string name;
    std::vector<ShaderUniform> uniforms;
    std::vector<ShaderData> shaders;
    std::vector<VertexAttribute> attributes;
    DrawParameters draw_parameters;
    size_t cache_entry{std::numeric_limits<size_t>::max()};
    uint64_t hash{0};

    static lib::Expected<Technique, lib::Result<TechniqueError>> load_from_file(std::filesystem::path const& file_path);
};

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

std::string constexpr get_shader_data_type_string(JSON_ShaderDataType const data_type);

ShaderDataType constexpr get_shader_data_type(JSON_ShaderDataType const data_type);

ShaderFlags constexpr get_shader_flags(JSON_ShaderType const shader_type);

FillMode constexpr get_fill_mode(JSON_FillMode const fill_mode);

CullMode constexpr get_cull_mode(JSON_CullMode const cull_mode);

}
