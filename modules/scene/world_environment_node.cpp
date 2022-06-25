// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/world_environment_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

WorldEnvironmentNode::WorldEnvironmentNode() {

}

void WorldEnvironmentNode::accept(SceneVisitor& visitor) {
    visitor(*this);
}

void WorldEnvironmentNode::skybox_material(asset::AssetPtr<asset::Material> material) {
    if(material->is_pending()) {
        material->wait();
    }
    _skybox_material = material;
}

asset::AssetPtr<asset::Material> WorldEnvironmentNode::skybox_material() const {
    return _skybox_material;
}
