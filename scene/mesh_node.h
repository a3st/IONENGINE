// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

//#include <scene/transform_node.h>
#include <asset/model.h>

namespace ionengine::scene {

class MeshNode {

    friend class Scene;

public:

    MeshNode() = default;

    void mesh(std::shared_ptr<asset::Model>& asset);

private:

    std::shared_ptr<asset::Model> _mesh;
};

}