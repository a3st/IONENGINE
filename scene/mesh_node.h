// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <asset/mesh.h>
#include <asset/material.h>

namespace ionengine::scene {

class MeshNode : public TransformNode {

    friend class SceneGraph;

public:

    MeshNode();

    virtual void accept(SceneVisitor& visitor);

    asset::Mesh const& mesh() const;

private:

    std::shared_ptr<asset::Mesh> _mesh;
    std::vector<std::shared_ptr<asset::Material>> _ovveride_materials;
};

}