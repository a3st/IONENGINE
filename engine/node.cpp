// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/node.h>

using namespace ionengine;

Node& Node::add_child(Node& child) {
        
    childrens.emplace_back(&child);
    return *this;
}

void Node::name(std::u8string_view const name) {

    _name = name;
}

std::u8string_view Node::name() const {
    
    return _name;
}