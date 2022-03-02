// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/texture_data.h>
#include <renderer/mesh_data.h>

namespace ionengine {

struct Asset {
    std::variant<renderer::MeshData, renderer::TextureData> data;
};

}