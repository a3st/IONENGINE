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

    void light_color(lib::math::Color const& color);

    lib::math::Color const& light_color() const;

    float light_range() const;

    float light_attenuation() const;

    void editor_icon(asset::AssetPtr<asset::Texture> const& texture);

    asset::AssetPtr<asset::Texture>& editor_icon();

private:

    lib::math::Color _light_color;
    float _light_range;
    float _light_attenuation;

    asset::AssetPtr<asset::Texture> _editor_icon;
};

}