// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>
#include <asset/buffer.h>

namespace ionengine::asset {

class Mesh : public Asset {
public:

    Mesh() = default;

    Mesh(VertexBuffer const& vertex_buffer, IndexBuffer const& index_buffer);

    static std::unique_ptr<Mesh> make_test_cube();

private:

    VertexBuffer _vertex_buffer;
    IndexBuffer _index_buffer;
};

}