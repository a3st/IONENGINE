// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct MaterialParameter {
    std::u8string name;
};

template<>
struct MaterialParameter<float> {
    std::u8string name;
    float value;
    std::pair<float, float> min_max;
};

template<>
struct MaterialParameter<Vector3f> {
    std::u8string name;
    Vector3f value;
};

template<>
struct MaterialParameter<Vector4f> {
    std::u8string name;
    Vector4f value;
};

template<>
struct MaterialParameter<Vector2f> {
    std::u8string name;
    Vector2f value;
};

using MaterialParameterDesc = std::variant<MaterialParameter<float>, MaterialParameter<Vector3f>, MaterialParameter<Vector4f>, MaterialParameter<Vector3f>>;

using MaterialId = uint32_t;

class MaterialManager {
public:

    MaterialManager() = default;

    void load_material_data(MaterialId const id, MaterialData const& material_data);

    //void build_materials();

private:



};

}