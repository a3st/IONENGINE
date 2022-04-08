// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene.h>

#include <asset/buffer.h>

namespace project {

class Sponza {
public:

    Sponza();

    ionengine::scene::Scene& scene() { return _scene; }

private:

    ionengine::scene::Scene _scene;
};

}