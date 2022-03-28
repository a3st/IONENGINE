// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/camera_node.h>

using namespace ionengine;

float CameraNode::aspect_ratio() const {

    return _aspect_ratio;
}

CameraNode& CameraNode::aspect_ratio(float const value) {

    _aspect_ratio = value;
    return *this;
}

float CameraNode::field_of_view() const {

    return _field_of_view;
}

CameraNode& CameraNode::field_of_view(float const value) {

    _field_of_view = value;
    return *this;
}