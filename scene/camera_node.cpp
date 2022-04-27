// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/camera_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

CameraNode::CameraNode() {

    _aspect_ratio = 4 / 3;
    _field_of_view = 60.0f * static_cast<float>(M_PI) / 180.0f;
}

float CameraNode::aspect_ratio() const {

    return _aspect_ratio;
}

void CameraNode::aspect_ratio(float const value) {

    _aspect_ratio = value;
}

float CameraNode::field_of_view() const {

    return _field_of_view;
}

void CameraNode::field_of_view(float const value) {

    _field_of_view = value;
}

void CameraNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

void CameraNode::calculate_matrices() {

    lib::math::Vector3f position = this->position();

    _view = lib::math::Matrixf::look_at(position, lib::math::Vector3f(0.0f, 0.0f, 0.0f), lib::math::Vector3f(0.0f, 1.0f, 0.0f)); 
    if(_projection_type == CameraProjectionType::Perspective) {
        _projection = lib::math::Matrixf::perspective(_field_of_view, _aspect_ratio, 0.1f, 100.0f);
    }
}

lib::math::Matrixf const& CameraNode::transform_view() const {

    return _view;
}

lib::math::Matrixf const& CameraNode::transform_projection() const {

    return _projection;
}
