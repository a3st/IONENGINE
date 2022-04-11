// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/mesh_node.h>

namespace ionengine::scene {

class Scene {
public:

    Scene();

    void add_mesh_node(MeshNode* node);

private:

    std::vector<MeshNode*> _meshes;
};

}
