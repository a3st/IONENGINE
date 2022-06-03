// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <scene/scene_graph.h>
#include <scene/camera_node.h>
#include <scene/streaming_scene_cache.h>

namespace ionengine::scene {

class Scene {
public:

    Scene();

    SceneGraph& graph();

    void update(float const delta_time);

    void load_subscene(asset::AssetPtr<asset::Subscene> subscene);

private:

    SceneGraph _scene_graph;

    StreamingSceneCache _subscene_cache;

    std::unique_ptr<SceneNode> _root_node;

    std::vector<CameraNode*> _camera_nodes_cache;
};

}
