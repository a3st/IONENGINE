// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/memory.h>
#include <lib/pool_allocator.h>

#include <scene/node.h>
#include <scene/scene_node.h>


namespace ionengine::scene {

class Scene {
public:

    Scene();

    template<class Type>
    Type& spawn_node(std::u8string_view const name, Type* parent = nullptr) {

        auto node = new Type();

        if(parent) {
            parent->add_child(*node);
        }

        return *node;
    }

    Node& resources_root() const {

        return *_resources_root;
    }

    Node& scene_root() const {

        return *_scene_root;
    }

private:

    Node* _root;
    Node* _scene_root;
    Node* _resources_root;
};

}
