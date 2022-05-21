// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/point_light_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

PointLightNode::PointLightNode() {
    _light_color = lib::math::Color(1.0f, 1.0f, 1.0f, 1.0f);
    _light_range = 2.0f;
    _light_attenuation = 0.9f;
}

void PointLightNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

void PointLightNode::light_color(lib::math::Color const& color) {
    _light_color = color;
}

lib::math::Color const& PointLightNode::light_color() const {
    return _light_color;
}

float PointLightNode::light_range() const {
    return _light_range;
}

float PointLightNode::light_attenuation() const {
    return _light_attenuation;
}

void PointLightNode::editor_icon(asset::AssetPtr<asset::Texture> const& texture) {
    _editor_icon = texture;
}

asset::AssetPtr<asset::Texture>& PointLightNode::editor_icon() {
    return _editor_icon;
}
