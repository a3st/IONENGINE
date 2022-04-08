// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene_node.h>

using ionengine::Handle;
using namespace ionengine::scene;

void SceneNode::add_child(Handle<SceneNode> const& child) {

    _childrens.emplace_back(child);
}

void SceneNode::name(std::string_view const name) {

    _name = name;
}

std::string_view SceneNode::name() const {

    return _name;
}

Handle<SceneNode> SceneNode::parent() const {

    return _parent;
}
