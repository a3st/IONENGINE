// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_ptr.h>
#include <asset/texture.h>
#include <lib/math/vector.h>
#include <lib/expected.h>
#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class JSON_MaterialParameterType {
    sampler2D,
    samplerCube,
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(JSON_MaterialParameterType, sampler2D, samplerCube, f32x4x4, f32x4, f32x3, f32x2, f32)

enum class JSON_MaterialDomain {
    surface
};

JSON5_ENUM(JSON_MaterialDomain, surface)

enum class JSON_MaterialBlendMode {
    opaque,
    transculent
};

JSON5_ENUM(JSON_MaterialBlendMode, opaque, transculent)

struct JSON_MaterialParameterValueDefinition {
    std::optional<std::string> path;
    std::optional<std::array<float, 4>> vec4;
    std::optional<std::array<float, 3>> vec3;
    std::optional<std::array<float, 2>> vec2;
    std::optional<float> value;
};

JSON5_CLASS(JSON_MaterialParameterValueDefinition, path, vec4, vec3, vec2, value)

struct JSON_MaterialParameterDefinition {
    std::string name;
    JSON_MaterialParameterType type;
    JSON_MaterialParameterValueDefinition value;
};

JSON5_CLASS(JSON_MaterialParameterDefinition, name, type, value)

struct JSON_MaterialPassDefinition {
    std::string name;
    std::string shader;
};

JSON5_CLASS(JSON_MaterialPassDefinition, name, shader)

struct JSON_MaterialDefinition {
    std::string name;
    JSON_MaterialDomain domain;
    JSON_MaterialBlendMode blend_mode;
    std::vector<JSON_MaterialParameterDefinition> parameters;
    std::vector<JSON_MaterialPassDefinition> passes;
};

JSON5_CLASS(JSON_MaterialDefinition, name, domain, blend_mode, parameters, passes)

namespace ionengine::asset {

class AssetManager;
struct Shader;

enum class MaterialError {
    IO,
    ParseError
};

enum class MaterialParameterType {
    Sampler2D,
    SamplerCube,
    F32x4x4,
    F32x4,
    F32x3,
    F32x2,
    F32
};

enum class MaterialDomain {
    Surface
};

enum class MaterialBlendMode {
    Opaque,
    Transculent
};

template<MaterialParameterType Type>
struct MaterialParameterData {};

template<>
struct MaterialParameterData<MaterialParameterType::Sampler2D> { 
    asset::AssetPtr<Texture> asset;
};

template<>
struct MaterialParameterData<MaterialParameterType::SamplerCube> { 
    asset::AssetPtr<Texture> asset;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32> {
    float value;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32x2> {
    lib::math::Vector2f value;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32x3> {
    lib::math::Vector3f value;
};

template<>
struct MaterialParameterData<MaterialParameterType::F32x4> {
    lib::math::Vector4f value;
};

#ifndef DECLARE_MATERIAL_PARAMETER_CAST
#define DECLARE_MATERIAL_PARAMETER_CAST(Name, Type) \
MaterialParameterData<Type>& Name() { \
    return std::get<MaterialParameterData<Type>>(data); \
}
#endif

struct MaterialParameter {
    std::variant<
        MaterialParameterData<MaterialParameterType::Sampler2D>,
        MaterialParameterData<MaterialParameterType::SamplerCube>,
        MaterialParameterData<MaterialParameterType::F32>,
        MaterialParameterData<MaterialParameterType::F32x2>,
        MaterialParameterData<MaterialParameterType::F32x3>,
        MaterialParameterData<MaterialParameterType::F32x4>
    > data;

    bool is_sampler2D() const { 
        return data.index() == 0; 
    }

    bool is_samplerCube() const { 
        return data.index() == 1; 
    }

    bool is_cbuffer() const {
        return !is_sampler2D() || !is_samplerCube();
    }

    DECLARE_MATERIAL_PARAMETER_CAST(as_sampler2D, MaterialParameterType::Sampler2D)
    DECLARE_MATERIAL_PARAMETER_CAST(as_samplerCube, MaterialParameterType::SamplerCube)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32, MaterialParameterType::F32)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x2, MaterialParameterType::F32x2)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x3, MaterialParameterType::F32x3)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x4, MaterialParameterType::F32x4)
};

struct Material {
    std::string name;
    MaterialDomain domain;
    MaterialBlendMode blend_mode;
    std::unordered_map<std::string, MaterialParameter> parameters;
    std::unordered_map<std::string, AssetPtr<Shader>> passes;

    uint64_t hash;

    static lib::Expected<Material, lib::Result<MaterialError>> load_from_file(std::filesystem::path const& file_path, AssetManager& asset_manager);
};

MaterialDomain constexpr get_material_domain(JSON_MaterialDomain const domain);

MaterialBlendMode constexpr get_material_blend_mode(JSON_MaterialBlendMode const blend_mode);

}
