// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>

namespace ionengine::asset {

enum class MdlVertexUsage : uint8_t {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    TexCoord0 = 3,
    TexCoord1 = 4,
    TexCoord2 = 5
};

class MdlVertex {

};

class MdlIndex {

};

class MdlSurface {
public:


private:

    

};

class Model : public Asset {
public:

    Model(std::filesystem::path const& file_path);


private:


};

}