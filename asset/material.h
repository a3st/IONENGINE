// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

#include <asset/texture.h>
#include <lib/math/vector.h>

namespace json5::detail {

inline json5::value write(writer& w, ionengine::lib::math::Vector4f const& in) {

	w.push_array();
	w += write(w, in.x);
	w += write(w, in.y);
	w += write(w, in.z);
    w += write(w, in.w);
	return w.pop();
}

inline error read(json5::value const& in, ionengine::lib::math::Vector4f& out) {

	return read(json5::array_view(in), out.x, out.y, out.z, out.w);
}

inline json5::value write(writer& w, ionengine::lib::math::Vector3f const& in) {

	w.push_array();
	w += write(w, in.x);
	w += write(w, in.y);
	w += write(w, in.z);
	return w.pop();
}

inline error read(json5::value const& in, ionengine::lib::math::Vector3f& out) {

	return read(json5::array_view(in), out.x, out.y, out.z);
}

inline json5::value write(writer& w, ionengine::lib::math::Vector2f const& in) {

	w.push_array();
	w += write(w, in.x);
	w += write(w, in.y);
	return w.pop();
}

inline error read(json5::value const& in, ionengine::lib::math::Vector2f& out) {

	return read(json5::array_view(in), out.x, out.y);
}

}

enum class JSON_MaterialParameterType {
    sampler2D,
    f32x4x4,
    f32x4,
    f32x3,
    f32x2,
    f32
};

JSON5_ENUM(JSON_MaterialParameterType, sampler2D, f32x4x4, f32x4, f32x3, f32x2, f32)

struct JSON_MaterialParameterValueDefinition {
    JSON_MaterialParameterType type;
    std::optional<std::string> path;
    std::optional<ionengine::lib::math::Vector4f> vec4;
    std::optional<ionengine::lib::math::Vector3f> vec3;
    std::optional<ionengine::lib::math::Vector2f> vec2;
    std::optional<float> value;
};

JSON5_CLASS(JSON_MaterialParameterValueDefinition, type, path, vec4, vec3, vec2, value)

struct JSON_MaterialParameterDefinition {
    std::string name;
    JSON_MaterialParameterValueDefinition value;
};

JSON5_CLASS(JSON_MaterialParameterDefinition, name, value)

struct JSON_MaterialPassParametersDefinition {
    
};

JSON5_CLASS(JSON_MaterialParameterDefinition)

struct JSON_MaterialPassDefinition {
    std::string name;
    JSON_MaterialPassParametersDefinition parameters;
    std::string technique;
};

JSON5_CLASS(JSON_MaterialParameterDefinition, name, parameters, technique)

struct JSON_MaterialDefinition {
    std::string name;
    std::vector<JSON_MaterialParameterDefinition> parameters;
    std::vector<JSON_MaterialPassDefinition> passes;
};

JSON5_CLASS(JSON_MaterialDefinition, name, parameters, passes)

namespace ionengine {
class AssetManager;
};

namespace ionengine::asset {

enum class MaterialParameterType {
    Sampler2D,
    F32x4x4,
    F32x4,
    F32x3,
    F32x2,
    F32
};

template<MaterialParameterType Type>
struct MaterialParameterData {};

template<>
struct MaterialParameterData<MaterialParameterType::Sampler2D> { 
    std::shared_ptr<Texture> asset;
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

    std::unordered_map<std::string, MaterialParameter> _parameters;
};

}
