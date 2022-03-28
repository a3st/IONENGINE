// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/node.h>

namespace ionengine {

class SceneTree {
public:

    SceneTree();

    template<class Type>
    Type& spawn_node(std::u8string_view const name, Type* parent = nullptr) {

        auto node = new Type();

        if(parent) {
            parent->add_child(*node);
        }

        return *node;
    }

private:

    Node* _root;
};

}
