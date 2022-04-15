
#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class UniformType {
    cbuffer,
    sampler2D
};

JSON5_ENUM(UniformType, cbuffer, sampler2D)

enum class DataType {
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(DataType, f32x4x4, f32x4, f32x3, f32x2, f32)

enum class ShaderType {
    vertex,
    geometry,
    hull,
    domain,
    pixel,
    compute
};

JSON5_ENUM(ShaderType, vertex, geometry, hull, domain, pixel, compute)

struct UniformDeclaration {
    std::string name;
    DataType type;
};

JSON5_CLASS(UniformDeclaration, name, type)

struct UniformData {
    std::string name;
    UniformType type;
    std::optional<std::vector<UniformDeclaration>> declaration;
    int location;
    std::optional<ShaderType> visibility;
};

JSON5_CLASS(UniformData, name, type, declaration, location, visibility)

struct InputOutputData {
    std::string name;
    DataType type;
    std::string semantic;
};

JSON5_CLASS(InputOutputData, name, type, semantic)

struct ShaderData {
    ShaderType type;
    std::vector<InputOutputData> inputs;
    std::vector<InputOutputData> outputs;
    std::string source;
};

JSON5_CLASS(ShaderData, type, inputs, outputs, source)

struct TechniqueData {
    std::string name;
    std::vector<UniformData> uniforms;
    std::vector<ShaderData> shaders;
};

JSON5_CLASS(TechniqueData, name, uniforms, shaders)

namespace ionengine::shader {

class TechniqueLoader {
public:

    TechniqueLoader() = default;

    bool parse(std::span<uint8_t const> const data);

private:
};

}
