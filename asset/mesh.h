// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>
#include <asset/buffer.h>

namespace ionengine::asset {

class Mesh : public Asset {
public:

    Mesh() = default;

private:

    VertexBuffer vertex_buffer;
    TriangleBuffer triangle_buffer;
};

}