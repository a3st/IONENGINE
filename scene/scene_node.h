// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/node.h>

#include <scene/components/camera.h>
#include <scene/components/static_mesh.h>

namespace ionengine::scene {

using Component = std::variant<components::Camera, components::StaticMesh>;

class SceneNode : public Node {
public:

    SceneNode() {}

    virtual ~SceneNode() { }

    virtual Node& add_child(Node& child);

    virtual void name(std::u8string_view const name);

    virtual std::u8string_view name() const;

private:

    struct Transform {
        lib::math::Vector3f position;
        lib::math::Vector3f scale;
    };

    std::optional<Component> _component;
};

}