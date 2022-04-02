// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/transform_node.h>

using namespace ionengine::scene;

void TransformNode::transform(Transform const& transform) {

    _transform = transform;

    _model_local = 
        lib::math::Matrixf().translate(transform.position) *
        lib::math::Quaternionf(transform.rotation).matrix() *
        lib::math::Matrixf().scale(transform.scale)
    ;
}

Transform const& TransformNode::transform() const {

    return _transform;
}