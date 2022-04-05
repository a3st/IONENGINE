// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/node_tree.h>

using ionengine::Handle;
using namespace ionengine::scene;

NodeTree::NodeTree() {

    _root = spawn_node<Node>(u8"root");
}

Handle<Node> NodeTree::root() const {

    return _root;
}