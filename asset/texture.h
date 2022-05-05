// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>

namespace ionengine::asset {

class Texture {
public:

    struct Size {
        uint32_t width;
        uint32_t height;
    };

    enum class Dimension {
        _2D,
        _3D,
        Cube
    };

    Texture() = default;

private:

    Size _size;

};

}