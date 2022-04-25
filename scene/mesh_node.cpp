// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/mesh_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

MeshNode::MeshNode() {

    _model = std::make_shared<asset::Model>("");
}

void MeshNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

asset::Surface const& MeshNode::surface() const {

    return _model->surfaces()[_surface_index];
}
