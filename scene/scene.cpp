// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>
#include <scene/mesh_node.h>
#include <scene/point_light_node.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::scene;

Scene::Scene() {
    _root_node = std::make_unique<TransformNode>();
    _root_node->name("root");
}

Scene::Scene(JSON_SubSceneDefinition const& document, asset::AssetManager& asset_manager) {

    std::unordered_map<std::string, SceneNode*> node_cache;
    node_cache.insert({ "root", _scene_graph.root() });
    
    for(auto const& node : document.nodes) {

        if(node.type == JSON_SubSceneNodeType::mesh) {

           
            
            auto allocated_node = _scene_graph.add_node<MeshNode>();
            allocated_node->name(node.name);
            allocated_node->mesh(asset_manager.get_mesh(mesh_info.mesh));

            for(size_t i = 0; i < mesh_info.materials.size(); ++i) {
                allocated_node->material(static_cast<uint32_t>(i), asset_manager.get_material(mesh_info.materials[i]));
            }

            allocated_node->position();

            

            

            allocated_node->scale();

            node_cache.insert({ node.name, allocated_node });

        } else if(node.type == JSON_SubSceneNodeType::point_light) {

            auto& point_light_info = node.values.point_light.value();

            auto allocated_node = _scene_graph.add_node<PointLightNode>();
            allocated_node->name(node.name);

            allocated_node->light_color();
            allocated_node->light_range(point_light_info.range);

            allocated_node->position(lib::math::Vector3f(node.values.position.at(0), node.values.position.at(1), node.values.position.at(2)));
           
            lib::math::Quaternionf quat_x = lib::math::Quaternionf::angle_axis(node.values.rotation.at(0), lib::math::Vector3f(1.0f, 0.0f, 0.0f));
            lib::math::Quaternionf quat_y = lib::math::Quaternionf::angle_axis(node.values.rotation.at(1), lib::math::Vector3f(0.0f, 1.0f, 0.0f));
            lib::math::Quaternionf quat_z = lib::math::Quaternionf::angle_axis(node.values.rotation.at(2), lib::math::Vector3f(0.0f, 0.0f, 1.0f));

            allocated_node->rotation(quat_x * quat_y * quat_z);

            allocated_node->scale(lib::math::Vector3f(node.values.scale.at(0), node.values.scale.at(1), node.values.scale.at(2)));

            allocated_node->editor_icon(asset_manager.get_texture("engine/editor/bulb.dds"));

            node_cache.insert({ node.name, allocated_node });
        }
    }

    for(auto const& link : document.links) {
        for(auto const& children : link.childrens) {
            node_cache.at(link.name)->add_child(node_cache.at(children));
        }
    }
}

SceneGraph& Scene::graph() {

    return _scene_graph;
}

void Scene::update(float const delta_time) {
    
}
