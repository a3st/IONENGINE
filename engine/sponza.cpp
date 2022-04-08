// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/sponza.h>

using namespace ionengine;
using namespace project;

Sponza::Sponza() {

    auto vase_001 = _scene.tree().spawn_node<scene::TransformNode>("vase_object", _scene.tree().root());
    
    scene::Transform transform = { 
        .position = lib::math::Vector3f(0.0f, 0.0f, 0.0f),
        .rotation = lib::math::Quaternionf{},
        .scale = lib::math::Vector3f(1.0f, 1.0f, 1.0f)
    };
    _scene.tree().node<scene::TransformNode>(vase_001).transform(transform);

    auto vase_mesh = _scene.tree().spawn_node<scene::MeshNode>("vase_mesh", vase_001);
    _scene.tree().node<scene::MeshNode>(vase_mesh).mesh(INVALID_HANDLE(asset::Asset));
}