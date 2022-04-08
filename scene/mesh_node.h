// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <asset/mesh.h>

namespace ionengine::scene {

class MeshNode : public TransformNode {

    friend class Scene;

public:

    MeshNode() = default;

    virtual void mesh(Handle<asset::Asset> const& asset);

private:

    Handle<asset::Asset> _mesh;
};

}