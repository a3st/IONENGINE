// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/node.h>

using ionengine::Handle;
using namespace ionengine::scene;

Node& Node::add_child(Handle<Node> const& child) {
        
    _childrens.emplace_back(child);
    return *this;
}

void Node::name(std::u8string_view const name) {

    _name = name;
}

std::u8string_view Node::name() const {
    
    return _name;
}

Handle<Node> Node::parent() const {

    return _parent;
}