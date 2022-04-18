// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class JSON_ShaderDataType {
    cbuffer,
    sampler2D,
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(JSON_ShaderDataType, cbuffer, sampler2D, f32x4x4, f32x4, f32x3, f32x2, f32)

enum class JSON_ShaderType {
    vertex,
    geometry,
    hull,
    domain,
    pixel,
    compute
};

JSON5_ENUM(JSON_ShaderType, vertex, geometry, hull, domain, pixel, compute)

struct JSON_ShaderStructDefinition {
    std::string name;
    JSON_ShaderDataType type;
    std::optional<std::string> semantic;
};

JSON5_CLASS(JSON_ShaderStructDefinition, name, type, semantic)

struct JSON_TechniqueUniformDefinition {
    std::string name;
    JSON_ShaderDataType type;
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

struct JSON_TechniqueDefinition {
    std::string name;
    std::vector<JSON_TechniqueUniformDefinition> uniforms;
    std::vector<JSON_TechniqueShaderDefinition> shaders;
};

JSON5_CLASS(JSON_TechniqueDefinition, name, uniforms, shaders)

namespace ionengine::asset {

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

enum class ShaderDataFlags : uint16_t {
    CBuffer = 1 << 0,
    Sampler2D = 1 << 1,
    F32x4x4 = 1 << 2,
    F32x4 = 1 << 3,
    F32x3 = 1 << 4,
    F32x2 = 1 << 5,
    F32 = 1 << 6,
    Constant = F32x4x4 | F32x4 | F32x3 | F32x2 | F32
};

DECLARE_ENUM_CLASS_BIT_FLAG(ShaderDataFlags)

template<ShaderDataFlags Flags>
struct ShaderUniformData {};

template<>
struct ShaderUniformData<ShaderDataFlags::Sampler2D> { };

struct ShaderBufferData {
    std::string name;
    ShaderDataFlags flags;
};

template<>
struct ShaderUniformData<ShaderDataFlags::CBuffer> {
    std::vector<ShaderBufferData> data;
};

template<>
struct ShaderUniformData<ShaderDataFlags::Constant> { 
    ShaderDataFlags flags;
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
        ShaderUniformData<ShaderDataFlags::Sampler2D>,
        ShaderUniformData<ShaderDataFlags::CBuffer>,
        ShaderUniformData<ShaderDataFlags::Constant>
    > data;

    ShaderFlags visibility;

    DECLARE_SHADER_UNIFORM_CAST(as_sampler2D, ShaderDataFlags::Sampler2D)
    DECLARE_SHADER_UNIFORM_CAST(as_cbuffer, ShaderDataFlags::CBuffer)
    DECLARE_SHADER_UNIFORM_CAST(as_constant, ShaderDataFlags::Constant)
};

struct ShaderData {
    std::string source;
    ShaderFlags flags;
};

class Technique {
public:

    Technique(std::filesystem::path const& file_path);

    std::string_view name() const;

    std::span<ShaderUniform const> uniforms() const;

    std::span<ShaderData const> data() const;

private:

    std::string _name;
    std::vector<ShaderUniform> _uniforms;
    std::vector<ShaderData> _data;

    std::string generate_uniform_code(
        std::string_view const name, 
        JSON_ShaderDataType const uniform_type, 
        uint32_t const location,
        std::optional<std::span<JSON_ShaderStructDefinition const>> const properties = std::nullopt
    ) const;

    std::string generate_struct_code(
        std::string_view const name, 
        std::span<JSON_ShaderStructDefinition const> const properties
    ) const;

    std::string constexpr get_shader_data_string(JSON_ShaderDataType const data_type) const;

    ShaderDataFlags constexpr get_shader_data_flags(JSON_ShaderDataType const data_type) const;

    ShaderFlags constexpr get_shader_flags(JSON_ShaderType const shader_type) const;
};

}
