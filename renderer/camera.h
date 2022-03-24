// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/matrix.h>

namespace ionengine::renderer {

class Camera {
public:

    Camera() = default;

    

private:

    Matrixf _view;
    Matrixf _projection;
};

}