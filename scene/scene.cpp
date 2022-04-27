// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>

#include <scene/mesh_node.h>

using namespace ionengine::scene;

Scene::Scene() {

}

SceneGraph& Scene::graph() {

    return _scene_graph;
}
