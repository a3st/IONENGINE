// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/transform_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine::scene;

void TransformNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

/*
void TransformNode::transform(Transform const& transform) {

    _transform = transform;
    
    _model_local = 
        lib::math::Matrixf().translate(transform.position) *
        lib::math::Quaternionf(transform.rotation).matrix() *
        lib::math::Matrixf().scale(transform.scale)
    ;
}*/