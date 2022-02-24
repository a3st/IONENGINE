// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <lib/math/vector.h>

namespace ionengine {

struct ObjMesh {
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> uvs;
    std::vector<uint32_t> indices;
};

class ObjLoader {
public:

    ObjLoader(std::span<char8_t> const data);

private:

};

}