// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <shader/technique.h>

enum class DrawFillMode {
    wireframe,
    solid
};

JSON5_ENUM(DrawFillMode, wireframe, solid)

enum class DrawCullMode {
    none,
    front,
    back
};

JSON5_ENUM(DrawCullMode, none, front, back)

struct DrawParameters {
    DrawFillMode fill_mode;
    DrawCullMode cull_mode;
    bool depth_stencil;
};

JSON5_CLASS(DrawParameters, fill_mode, cull_mode, depth_stencil)

struct TechniqueImportDefinition {
    std::string import;
    std::string from;
};

JSON5_CLASS(TechniqueImportDefinition, import, from)

struct ShaderPassDefinition {
    std::string name;
    DrawParameters draw_parameters;
    TechniqueImportDefinition technique;
};

JSON5_CLASS(ShaderPassDefinition, name, draw_parameters, technique)

struct ShaderPropertyDefinition {
    std::string name;
    ShaderUniformType type;
    std::optional<std::string> from;
};

JSON5_CLASS(ShaderPropertyDefinition, name, type, from)

struct ShaderData {
    std::string name;
    std::vector<ShaderPropertyDefinition> properties;
    std::vector<ShaderPassDefinition> shader_passes;
};

JSON5_CLASS(ShaderData, name, properties, shader_passes)

namespace ionengine::shader {

class Shader {
public:

    Shader(std::filesystem::path const& file_path);

    ShaderData const& data() const;

private:

    ShaderData _data;
};

}
