// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

struct OBJIndex {
    uint32_t vertex;
    uint32_t uv;
    uint32_t normal;
};

struct OBJMaterial {
    std::u8string name;
    std::vector<OBJIndex> indices;
};

struct OBJObject {
    std::u8string name;
    std::vector<OBJMaterial> materials;
};

struct OBJData {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<OBJObject> objects;
};

class OBJLoader {
public:

    OBJLoader() = default;

    bool parse(std::span<char8_t const> const data);

    OBJData const& data() const { return _data; }

private:

    OBJData _data;
};

}
