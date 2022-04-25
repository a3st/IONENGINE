// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <asset/model.h>
#include <asset/material.h>

namespace ionengine::scene {

class MeshNode : public TransformNode {
public:

    MeshNode();

    virtual void accept(SceneVisitor& visitor);

    asset::Surface const& surface() const;

private:

    std::shared_ptr<asset::Model> _model;
    uint32_t _surface_index{0};
    std::vector<std::shared_ptr<asset::Material>> _materials;
};

}