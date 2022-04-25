// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene_node.h>

using namespace ionengine::scene;

void SceneNode::name(std::string_view const name) {

    _name = name;
}

 
std::string_view SceneNode::name() const {

    return _name;
}

void SceneNode::add_child(SceneNode* const node) {

}

SceneNode* SceneNode::parent() {

    return _parent;
}
