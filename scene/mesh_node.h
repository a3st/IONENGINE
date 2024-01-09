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

    void mesh(asset::AssetPtr<asset::Mesh> mesh);

    asset::AssetPtr<asset::Mesh> mesh();

    void material(uint32_t const index, asset::AssetPtr<asset::Material> material);

    asset::AssetPtr<asset::Material> material(uint32_t const index);

private:

    asset::AssetPtr<asset::Mesh> _mesh;
    std::vector<asset::AssetPtr<asset::Material>> _override_materials;
};

}