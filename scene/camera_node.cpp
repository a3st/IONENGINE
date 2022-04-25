// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/camera_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine::scene;

CameraNode::CameraNode() {

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
