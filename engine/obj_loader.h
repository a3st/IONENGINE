// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>

namespace ionengine {

struct ObjIndex {
    uint32_t vertex;
    uint32_t uv;
    uint32_t normal;
};

struct ObjMesh {
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> uvs;
    std::vector<ObjIndex> indices;
};

class ObjLoader {
public:

    ObjLoader() = default;

    bool parse(std::span<char8_t> const data);

    std::vector<ObjMesh>::const_iterator begin() const { return _meshes.begin(); }
    std::vector<ObjMesh>::const_iterator end() const { return _meshes.end(); }

private:

    std::vector<ObjMesh> _meshes;
};

}