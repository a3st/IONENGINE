// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <asset/asset_ptr.h>
#include <asset/texture.h>
#include <lib/math/color.h>

namespace ionengine::scene {

class PointLightNode : public TransformNode {
public:

    PointLightNode();

    virtual void accept(SceneVisitor& visitor);

    void color(lib::math::Color const& color);

    lib::math::Color const& color() const;

    float range() const;

    void range(float const range);

private:

    lib::math::Color _color;
    float _range;
};

}