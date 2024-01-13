// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <asset/asset_ptr.h>
#include <asset/material.h>

namespace ionengine::scene {

class WorldEnvironmentNode : public SceneNode {
public:

    WorldEnvironmentNode();

    virtual void accept(SceneVisitor& visitor);

    void skybox_material(asset::AssetPtr<asset::Material> material);

    asset::AssetPtr<asset::Material> skybox_material() const;

private:

    asset::AssetPtr<asset::Material> _skybox_material;
};

}
