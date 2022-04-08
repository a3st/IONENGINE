// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene_tree.h>

using ionengine::Handle;
using namespace ionengine::scene;

SceneTree::SceneTree() {

    _root = spawn_node<SceneNode>("root");
}

Handle<SceneNode> SceneTree::root() const {

    return _root;
}
