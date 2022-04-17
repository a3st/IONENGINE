// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

#include <asset/shader.h>
#include <asset/texture.h>

#include <lib/math/vector.h>

enum class JSON_MaterialParameterType {
    sampler2D,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(JSON_MaterialParameterType, sampler2D, f32x4, f32x3, f32x2, f32)

struct JSON_MaterialPropertyDefinition {
    JSON_MaterialParameterType type;
    std::optional<std::string> asset;
    std::optional<std::vector<float>> value;
};

JSON5_CLASS(JSON_MaterialPropertyDefinition, type, asset, value)

namespace ionengine::asset {

enum class MaterialParameterType {
    Sampler2D,
    F32x4,
    F32x3,
    F32x2,
    F32
};

template<MaterialParameterType Type>
struct MaterialParameter {};

template<>
struct MaterialParameter<MaterialParameterType::Sampler2D> { 
    std::shared_ptr<Texture> asset;
};

template<>
struct MaterialParameter<MaterialParameterType::F32> {
    float value;
};

template<>
struct MaterialParameter<MaterialParameterType::F32x2> {
    lib::math::Vector2f value;
};

template<>
struct MaterialParameter<MaterialParameterType::F32x3> {
    lib::math::Vector3f value;
};

template<>
struct MaterialParameter<MaterialParameterType::F32x4> {
    lib::math::Vector4f value;
};

#ifndef DECLARE_MATERIAL_PARAMETER_CAST
#define DECLARE_MATERIAL_PARAMETER_CAST(Name, Type) \
MaterialParameter<Type> const& Name() { \
    return std::get<MaterialParameter<Type>>(property); \
}
#endif

struct MaterialProperty {
    std::variant<
        MaterialParameter<MaterialParameterType::Sampler2D>,
        MaterialParameter<MaterialParameterType::F32>,
        MaterialParameter<MaterialParameterType::F32x2>,
        MaterialParameter<MaterialParameterType::F32x3>,
        MaterialParameter<MaterialParameterType::F32x4>
    > property;

    DECLARE_MATERIAL_PARAMETER_CAST(as_sampler2D, MaterialParameterType::Sampler2D)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32, MaterialParameterType::F32)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x2, MaterialParameterType::F32x2)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x3, MaterialParameterType::F32x3)
    DECLARE_MATERIAL_PARAMETER_CAST(as_f32x4, MaterialParameterType::F32x4)
};

class Material {
public:
    
    Material(std::filesystem::path const& file_path, AssetManager& asset_manager);

private:

    std::shared_ptr<Shader> _shader;
    std::unordered_map<std::string, MaterialProperty> _properties;
};

}
