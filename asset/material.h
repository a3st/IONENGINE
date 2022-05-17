// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_ptr.h>
#include <asset/texture.h>
#include <asset/technique.h>
#include <lib/math/vector.h>
#include <lib/expected.h>
#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class JSON_MaterialParameterType {
    sampler2D,
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(JSON_MaterialParameterType, sampler2D, f32x4x4, f32x4, f32x3, f32x2, f32)

enum class JSON_PassFillMode {
    wireframe,
    solid
};

JSON5_ENUM(JSON_PassFillMode, wireframe, solid)

enum class JSON_PassCullMode {
    front,
    back,
    none
};

JSON5_ENUM(JSON_PassCullMode, front, back, none)

struct JSON_MaterialParameterValueDefinition {
    std::optional<std::string> path;
    std::optional<std::array<float, 4>> vec4;
    std::optional<std::array<float, 3>> vec3;
    std::optional<std::array<float, 2>> vec2;
    std::optional<float> value;
    std::optional<std::string> uniform;
    std::optional<int> offset;
};

JSON5_CLASS(JSON_MaterialParameterValueDefinition, path, vec4, vec3, vec2, value, uniform, offset)

struct JSON_MaterialParameterDefinition {
    std::string name;
    JSON_MaterialParameterType type;
    JSON_MaterialParameterValueDefinition value;
};

JSON5_CLASS(JSON_MaterialParameterDefinition, name, type, value)

struct JSON_MaterialPassParametersDefinition {
    JSON_PassFillMode fill_mode;
    JSON_PassCullMode cull_mode;
    bool depth_stencil;
};

JSON5_CLASS(JSON_MaterialPassParametersDefinition, fill_mode, cull_mode, depth_stencil)

struct JSON_MaterialPassDefinition {
    std::string name;
    JSON_MaterialPassParametersDefinition parameters;
    std::string technique;
};

JSON5_CLASS(JSON_MaterialPassDefinition, name, parameters, technique)

struct JSON_MaterialDefinition {
    std::string name;
    std::vector<JSON_MaterialParameterDefinition> parameters;
    std::vector<JSON_MaterialPassDefinition> passes;
};

JSON5_CLASS(JSON_MaterialDefinition, name, parameters, passes)

namespace ionengine::asset {

class AssetManager;

enum class MaterialError {
    IO,
    ParseError
};

enum class MaterialParameterType {
    Sampler2D,
    F32x4x4,
    F32x4,
    F32x3,
    F32x2,
    F32
};

enum class MaterialPassFillMode {
    Wireframe,
    Solid
};

enum class MaterialPassCullMode {
    Front,
    Back,
    None
};

template<MaterialParameterType Type>
struct MaterialParameterData {};

template<>
struct MaterialParameterData<MaterialParameterType::Sampler2D> { 
    asset::AssetPtr<Texture> asset;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32> {
    float value;
    std::string uniform;
    uint64_t offset;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32x2> {
    lib::math::Vector2f value;
    std::string uniform;
    uint64_t offset;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32x3> {
    lib::math::Vector3f value;
    std::string uniform;
    uint64_t offset;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32x4> {
    lib::math::Vector4f value;
    std::string uniform;
    uint64_t offset;
};

#ifndef DECLARE_MATERIAL_PARAMETER_CAST
#define DECLARE_MATERIAL_PARAMETER_CAST(Name, Type) \
MaterialParameterData<Type> const& Name() { \
    return std::get<MaterialParameterData<Type>>(data); \
}
#endif

struct MaterialParameter {
    std::variant<
        MaterialParameterData<MaterialParameterType::Sampler2D>,
        MaterialParameterData<MaterialParameterType::F32>,
        MaterialParameterData<MaterialParameterType::F32x2>,
        MaterialParameterData<MaterialParameterType::F32x3>,
        MaterialParameterData<MaterialParameterType::F32x4>
    > data;

    bool is_sampler2D() const { 
        return data.index() == 0; 
    }

    bool is_cbuffer() const {
        return data.index() != 0;
    }

    DECLARE_MATERIAL_PARAMETER_CAST(as_sampler2D, MaterialParameterType::Sampler2D)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32, MaterialParameterType::F32)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x2, MaterialParameterType::F32x2)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x3, MaterialParameterType::F32x3)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x4, MaterialParameterType::F32x4)
};

struct MaterialPassParameters {
    MaterialPassFillMode fill_mode;
    MaterialPassCullMode cull_mode;
};

struct MaterialPass {
    std::string name;
    asset::AssetPtr<asset::Technique> technique;
    MaterialPassParameters parameters;
};

class Material {
public:
    
    static lib::Expected<Material, lib::Result<MaterialError>> load_from_file(std::filesystem::path const& file_path, AssetManager& asset_manager);

    std::string_view name() const;

    std::span<MaterialPass const> passes() const;

    std::unordered_map<std::string, MaterialParameter>& parameters();

private:

    Material(JSON_MaterialDefinition const& document, AssetManager& asset_manager);

    std::string _name;
    std::unordered_map<std::string, MaterialParameter> _parameters;
    std::vector<MaterialPass> _passes;

    MaterialPassFillMode constexpr get_pass_fill_mode(JSON_PassFillMode const fill_mode) const;

    MaterialPassCullMode constexpr get_pass_cull_mode(JSON_PassCullMode const cull_mode) const;
};

}
