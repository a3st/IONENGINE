// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>

namespace ionengine::renderer {

using MaterialId = uint32_t;

class MaterialManager {
public:

    MaterialManager() = default;

    void load_material_data(MaterialId const id, MaterialData const& material_data);

    //void build_materials();

private:



};

}