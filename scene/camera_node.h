// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <asset/texture.h>
#include <asset/asset_ptr.h>
#include <lib/math/matrix.h>

namespace ionengine::scene {

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class CameraNode : public TransformNode {
public:

    CameraNode();

    float aspect_ratio() const;

    void aspect_ratio(float const value);

    float field_of_view() const;

    void field_of_view(float const value);

    virtual void accept(SceneVisitor& visitor);

    void calculate_matrices();

    lib::math::Matrixf const& transform_view() const;

    lib::math::Matrixf const& transform_projection() const;

private:

    CameraProjectionType _projection_type{CameraProjectionType::Perspective};

    lib::math::Matrixf _view;
    lib::math::Matrixf _projection;

    float _aspect_ratio;
    float _field_of_view;

    asset::AssetPtr<asset::Texture> _render_target;
};

}
