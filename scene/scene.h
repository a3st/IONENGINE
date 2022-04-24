// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/node.h>

namespace ionengine::scene {

class Scene {
public:

    Scene();

private:

    std::vector<std::unique_ptr<Node>> _meshes;
};

}
