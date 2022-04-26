// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/transform_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

TransformNode::TransformNode() {

    _model_local = lib::math::Matrixf::identity();
    _model_global = lib::math::Matrixf::identity();
}

void TransformNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

void TransformNode::position(lib::math::Vector3f const& position) {
    
    _position = position;
}

lib::math::Vector3f const& TransformNode::position() const {
    
    return _position;
}

void TransformNode::rotation(lib::math::Quaternionf const& rotation) {
    
    _rotation = rotation;
}

lib::math::Quaternionf const& TransformNode::rotation() const {
    
    return _rotation;
}

void TransformNode::scale(lib::math::Vector3f const& scale) {
    
    _scale = scale;
}

lib::math::Vector3f const& TransformNode::scale() const {
    
    return _scale;
}
