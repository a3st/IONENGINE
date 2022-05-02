// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/mesh_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

MeshNode::MeshNode() {

    
}

void MeshNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

asset::Mesh const& MeshNode::mesh() const {

    return *_mesh;
}
