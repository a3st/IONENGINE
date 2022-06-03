// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/streaming_scene.h>
#include <scene/camera_node.h>
#include <scene/mesh_node.h>
#include <scene/transform_node.h>
#include <scene/point_light_node.h>

using namespace ionengine;
using namespace ionengine::scene;

lib::Expected<StreamingScene, lib::Result<StreamingSceneError>> StreamingScene::load_from_subscene(asset::Subscene const& subscene) {

    auto streaming_scene = StreamingScene {};

    std::unordered_map<std::string, SceneNode*> node_names_cache;

    for(auto const& node : subscene.nodes) {

        std::unique_ptr<SceneNode> alloc_node = nullptr;

        auto node_visitor = make_visitor(
            [&](asset::SubsceneNodeData<asset::SubsceneNodeType::Empty> const& data) {
                alloc_node = std::make_unique<TransformNode>();
                auto casted_node = static_cast<TransformNode*>(alloc_node.get());
                casted_node->position(data.position);
                casted_node->rotation(data.rotation);
                casted_node->scale(data.scale);
            },
            [&](asset::SubsceneNodeData<asset::SubsceneNodeType::Mesh> const& data) {
                alloc_node = std::make_unique<MeshNode>();
                auto casted_node = static_cast<MeshNode*>(alloc_node.get());
                casted_node->mesh(data.mesh);
                for(size_t i = 0; i < data.materials.size(); ++i) {
                    casted_node->material(static_cast<uint32_t>(i), data.materials.at(i));
                }
                casted_node->position(data.position);
                casted_node->rotation(data.rotation);
                casted_node->scale(data.scale);
            },
            [&](asset::SubsceneNodeData<asset::SubsceneNodeType::PointLight> const& data) {
                alloc_node = std::make_unique<PointLightNode>();
                auto casted_node = static_cast<PointLightNode*>(alloc_node.get());
                casted_node->color(data.color);
                casted_node->range(data.range);
                casted_node->position(data.position);
                casted_node->rotation(data.rotation);
                casted_node->scale(data.scale);
            }
        );

        std::visit(node_visitor, node.data);

        alloc_node->name(node.name);

        node_names_cache.insert({ node.name, alloc_node.get() });
        streaming_scene.nodes.push_back(std::move(alloc_node));
    }

    for(auto const& node : subscene.nodes) {
        for(auto const& children : subscene.links.at(node.name)) {
            node_names_cache.at(node.name)->add_child(node_names_cache.at(children));
        }
    }

    return lib::Expected<StreamingScene, lib::Result<StreamingSceneError>>::ok(std::move(streaming_scene));
}
