// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/point_light_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

PointLightNode::PointLightNode() {
    _color = lib::math::Color(1.0f, 1.0f, 1.0f, 1.0f);
    _range = 1.0f;
}

void PointLightNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

void PointLightNode::color(lib::math::Color const& color) {
    _color = color;
}

lib::math::Color const& PointLightNode::color() const {
    return _color;
}

float PointLightNode::range() const {
    return _range;
}

void PointLightNode::range(float const range) {
    _range = range;
}

void PointLightNode::editor_icon(asset::AssetPtr<asset::Texture> texture) {
    if(texture.is_pending()) {
        texture.wait();
    }
    _editor_icon = texture;
}

asset::AssetPtr<asset::Texture>& PointLightNode::editor_icon() {
    return _editor_icon;
}
