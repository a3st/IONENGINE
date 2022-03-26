// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>

using namespace ionengine::scene;

Scene::Scene() {

    Node& root_node = spawn_node<Node>(u8"root");

    Node& resources_node = spawn_node<Node>(u8"resources", &root_node);

    Node& components_node = spawn_node<Node>(u8"components", &root_node);

    Node& scene_node = spawn_node<Node>(u8"scene", &root_node);

    _root = &root_node;
    _scene_root = &scene_node;
    _resources_root = &resources_node;
}
