// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <scene/scene_graph.h>
#include <scene/camera_node.h>
#include <scene/subscene_cache.h>

namespace ionengine {

namespace asset {
class AssetManager;
};

}

namespace ionengine::scene {

class Scene {
public:

    Scene(asset::AssetManager& asset_manager);

    SceneGraph& graph();

    void update(float const delta_time);

    void load(std::filesystem::path const& subscene_path);

    void visit_culling_nodes(SceneVisitor& visitor);

    void print_structure();

private:

    asset::AssetManager* _asset_manager;

    std::unique_ptr<SceneNode> _root_node;

    SceneGraph _scene_graph;

    SubsceneCache _subscene_cache;

    std::vector<Subscene*> loaded_subscenes;

    std::vector<CameraNode*> _camera_nodes_cache;
};

}
