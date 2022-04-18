// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

#include <asset/technique.h>

enum class JSON_ShaderParameterType {
    sampler2D,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(JSON_ShaderParameterType, sampler2D, f32x4, f32x3, f32x2, f32)

enum class JSON_DrawParameterFillMode {
    wireframe,
    solid
};

JSON5_ENUM(JSON_DrawParameterFillMode, wireframe, solid)

enum class JSON_DrawParameterCullMode {
    none,
    front,
    back
};

JSON5_ENUM(JSON_DrawParameterCullMode, none, front, back)

struct JSON_ShaderParameterDefinition {
    std::string name;
    JSON_ShaderParameterType type;
    std::string from;
};

JSON5_CLASS(JSON_ShaderParameterDefinition, name, type, from)

struct JSON_DrawParametersDefinition {
    JSON_DrawParameterFillMode fill_mode;
    JSON_DrawParameterCullMode cull_mode;
    bool depth_stencil;
};

JSON5_CLASS(JSON_DrawParametersDefinition, fill_mode, cull_mode, depth_stencil)

struct JSON_TechniqueImportDefinition {
    std::string import;
    std::string from;
};

JSON5_CLASS(JSON_TechniqueImportDefinition, import, from)

struct JSON_ShaderPassDefinition {
    std::string name;
    JSON_DrawParametersDefinition draw_parameters;
    JSON_TechniqueImportDefinition technique;
};

JSON5_CLASS(JSON_ShaderPassDefinition, name, draw_parameters, technique)

struct JSON_ShaderDefinition {
    std::string name;
    std::vector<JSON_ShaderParameterDefinition> parameters;
    std::vector<JSON_ShaderPassDefinition> passes;
};

JSON5_CLASS(JSON_ShaderDefinition, name, parameters, passes)

namespace ionengine {
class AssetManager;
}

namespace ionengine::asset {

enum class ShaderParameterType {
    Sampler2D,
    F32x4,
    F32x3,
    F32x2,
    F32
};

struct ShaderParameter {
    std::string name;
    ShaderParameterType type;
    std::string from;
};

enum class DrawParameterFillMode {
    Wireframe,
    Solid
};

enum class DrawParameterCullMode {
    None,
    Front,
    Back
};

struct DrawParameters {
    DrawParameterFillMode fill_mode;
    DrawParameterCullMode cull_mode;
    bool depth_stencil;
};

struct ShaderPass {
    std::string name;
    DrawParameters draw_parameters;
    std::shared_ptr<Technique> technique;
};

class Shader {
public:

    Shader(std::filesystem::path const& file_path, AssetManager& asset_manager);

    std::string_view name() const;

    std::span<ShaderParameter const> parameters() const;

    std::span<ShaderPass const> shader_passes() const;

private:

    std::string _name;
    std::vector<ShaderParameter> _parameters;
    std::vector<ShaderPass> _shader_passes;
};

}
