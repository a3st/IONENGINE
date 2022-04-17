// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class ShaderUniformType {
    cbuffer,
    sampler2D
};

JSON5_ENUM(ShaderUniformType, cbuffer, sampler2D)

enum class ShaderType {
    vertex,
    geometry,
    hull,
    domain,
    pixel,
    compute
};

JSON5_ENUM(ShaderType, vertex, geometry, hull, domain, pixel, compute)

enum class ShaderDataType {
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(ShaderDataType, f32x4x4, f32x4, f32x3, f32x2, f32)

struct ShaderUniformPropertyDefinition {
    std::string name;
    ShaderDataType type;
};

JSON5_CLASS(ShaderUniformPropertyDefinition, name, type)

struct TechniquePropertyDefinition {
    std::string name;
    ShaderUniformType type;
    std::optional<std::vector<ShaderUniformPropertyDefinition>> properties;
    int location;
    std::optional<ShaderType> visibility;
};

JSON5_CLASS(TechniquePropertyDefinition, name, type, properties, location, visibility)

struct VertexAssemblerDefinition {
    std::string name;
    ShaderDataType type;
    std::string semantic;
};

JSON5_CLASS(VertexAssemblerDefinition, name, type, semantic)

struct TechniqueShaderDefinition {
    ShaderType type;
    std::vector<VertexAssemblerDefinition> inputs;
    std::vector<VertexAssemblerDefinition> outputs;
    std::string source;
};

JSON5_CLASS(TechniqueShaderDefinition, type, inputs, outputs, source)

struct TechniqueData {
    std::string name;
    std::vector<TechniquePropertyDefinition> properties;
    std::vector<TechniqueShaderDefinition> shaders;
};

JSON5_CLASS(TechniqueData, name, properties, shaders)

namespace ionengine::asset {

class Technique {
public:

    Technique(std::filesystem::path const& file_path);

    TechniqueData const& data() const;

private:

    TechniqueData _data;

    std::string generate_uniform_code(
        std::string_view const name, 
        ShaderUniformType const uniform_type, 
        uint32_t const location,
        std::optional<std::span<ShaderUniformPropertyDefinition const>> const properties = std::nullopt
    );

    std::string generate_vertex_assembler_code(
        std::string_view const name, 
        std::span<VertexAssemblerDefinition const> const properties
    );

    std::string constexpr get_shader_data_type(ShaderDataType const data_type);
};

}