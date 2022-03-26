// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <lib/math/vector.h>

namespace ionengine::core {

class Asset;

}

namespace ionengine::scene::resources {

template<class Type>
struct MaterialParameterType { };

template<>
struct MaterialParameterType<float> {
    float value;
    std::pair<float, float> min_max;
};

template<>
struct MaterialParameterType<Vector3f> {
    Vector3f value;
};

template<>
struct MaterialParameterType<Vector4f> {
    Vector4f value;
};

template<>
struct MaterialParameterType<Vector2f> {
    Vector2f value;
};

struct Sampler;

template<>
struct MaterialParameterType<Sampler> {
    Handle<core::Asset> sampler;
};

using MaterialParameter = std::variant<
    MaterialParameterType<float>, 
    MaterialParameterType<Vector3f>, 
    MaterialParameterType<Vector4f>, 
    MaterialParameterType<Vector3f>, 
    MaterialParameterType<Sampler>
>;

class Material {
public:
    
    Material() = default;

    Material(Handle<core::Asset> const& material_asset, Handle<core::Asset> const& shader_asset);

private:

    Handle<core::Asset> shader;
    Handle<core::Asset> material;

    std::unordered_map<std::u8string, MaterialParameter> _parameters;
};

}