// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/scene_tree.h>

using namespace ionengine;

SceneTree::SceneTree() {

    Node& root_node = spawn_node<Node>(u8"root");
    _root = &root_node;
}
