// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>
#include <scene/mesh_node.h>
#include <scene/point_light_node.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::scene;

Scene::Scene(asset::AssetManager& asset_manager) :
    _asset_manager(&asset_manager),
    _root_node(std::make_unique<TransformNode>()),
    _scene_graph(_root_node.get()) {

    _root_node->name("root");
}

void Scene::load(std::filesystem::path const& subscene_path) {

    Subscene& subscene = _subscene_cache.get(*_asset_manager, subscene_path);
    
    _scene_graph.root()->add_child(subscene.root_node);
    _nodes.emplace_back(subscene.root_node);

    loaded_subscenes.emplace_back(&subscene);
}

SceneGraph& Scene::graph() {
    return _scene_graph;
}

void Scene::update(float const delta_time) {
    _scene_graph.update_hierarchical_data();
}

void Scene::print_structure() {
    
    for(auto const& node : _scene_graph.root()->childrens()) {

        std::cout << node->name() << std::endl;
    }
}

void Scene::visit_culling_nodes(SceneVisitor& visitor) {

    for(auto const& subscene : loaded_subscenes) {
        for(auto const& node : subscene->nodes) {
            node->accept(visitor);
        }
    }
}
