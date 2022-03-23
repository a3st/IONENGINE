// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <lib/math/vector.h>
#include <renderer/material.h>

namespace ionengine::renderer {

enum class ShaderDomain {
    Surface,
    Decal,
    PostProcess
};

enum class ShaderBlendMode {
    Opaque,
    Transparent
};

enum class ShaderQuality {
    Low,
    High
};

using MaterialId = uint32_t;

class MaterialManager {
public:

    MaterialManager() = default;

    void load_material_data(MaterialId const id, MaterialData const& material_data);

    //void build_materials();

private:



};

}