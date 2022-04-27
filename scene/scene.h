// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <scene/scene_graph.h>

namespace ionengine::scene {

class Scene {
public:

    Scene();

    SceneGraph& graph();

private:

    SceneGraph _scene_graph;
};

}
