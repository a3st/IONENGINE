// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/subscene.h>
#include <scene/camera_node.h>
#include <scene/mesh_node.h>
#include <scene/transform_node.h>
#include <scene/point_light_node.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::scene;

lib::Expected<Subscene, lib::Result<SubsceneError>> Subscene::load_from_file(std::filesystem::path const& file_path, asset::AssetManager& asset_manager) {

    std::string path_string = file_path.string();

    JSON_SubSceneDefinition document;
    json5::error result = json5::from_file(path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::make_expected<Subscene, lib::Result<SubsceneError>>(
            lib::Result<SubsceneError> { .errc = SubsceneError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::make_expected<Subscene, lib::Result<SubsceneError>>(
            lib::Result<SubsceneError> { .errc = SubsceneError::ParseError, .message = "Parse file error" }
        );
    }

    auto subscene = Subscene {};
    subscene.name = document.name;

    std::unordered_map<std::string, SceneNode*> node_names_cache;
    
    for(auto const& node : document.nodes) {

        std::unique_ptr<SceneNode> alloc_node = nullptr;

        auto position = lib::math::Vector3f(node.values.position.at(0), node.values.position.at(1), node.values.position.at(2));

        auto quat_x = lib::math::Quaternionf::angle_axis(node.values.rotation.at(0), lib::math::Vector3f(1.0f, 0.0f, 0.0f));
        auto quat_y = lib::math::Quaternionf::angle_axis(node.values.rotation.at(1), lib::math::Vector3f(0.0f, 1.0f, 0.0f));
        auto quat_z = lib::math::Quaternionf::angle_axis(node.values.rotation.at(2), lib::math::Vector3f(0.0f, 0.0f, 1.0f));
        auto rotation = quat_x * quat_y * quat_z;

        auto scale = lib::math::Vector3f(node.values.scale.at(0), node.values.scale.at(1), node.values.scale.at(2));

        switch(node.type) {

            case JSON_SubSceneNodeType::empty: {

                alloc_node = std::make_unique<TransformNode>();
                auto casted_node = static_cast<TransformNode*>(alloc_node.get());
                casted_node->position(position);
                casted_node->rotation(rotation);
                casted_node->scale(scale);

            } break;

            case JSON_SubSceneNodeType::mesh: {

                auto& mesh_node_data = node.values.mesh.value();

                alloc_node = std::make_unique<MeshNode>();
                auto casted_node = static_cast<MeshNode*>(alloc_node.get());
                casted_node->mesh(asset_manager.get_mesh(mesh_node_data.mesh));
                for(size_t i = 0; i < mesh_node_data.materials.size(); ++i) {
                    casted_node->material(static_cast<uint32_t>(i), asset_manager.get_material(mesh_node_data.materials.at(i)));
                }
                casted_node->position(position);
                casted_node->rotation(rotation);
                casted_node->scale(scale);

            } break;

            case JSON_SubSceneNodeType::point_light: {

                auto& point_light_data = node.values.point_light.value();

                auto color = lib::math::Color(point_light_data.color.at(0), point_light_data.color.at(1), point_light_data.color.at(2), 1.0f);

                alloc_node = std::make_unique<PointLightNode>();
                auto casted_node = static_cast<PointLightNode*>(alloc_node.get());
                casted_node->color(color);
                casted_node->range(point_light_data.range);
                casted_node->position(position);
                casted_node->rotation(rotation);
                casted_node->scale(scale);

            } break;
        }

        if(node.name == "root") {
            alloc_node->name(std::format("{}_root", document.name));
            subscene.root_node = alloc_node.get();
        } else {
            alloc_node->name(node.name);
        }

        node_names_cache.insert({ std::string(alloc_node->name().begin(), alloc_node->name().end()), alloc_node.get() });
        subscene.nodes.push_back(std::move(alloc_node));
    }
    
    for(auto const& link : document.links) {
        
        for(auto const& children : link.childrens) {

            if(link.name == "root") {
                node_names_cache.at(std::format("{}_root", document.name))->add_child(node_names_cache.at(children));
            } else {
                node_names_cache.at(link.name)->add_child(node_names_cache.at(children));
            }
        }
    }

    return lib::make_expected<Subscene, lib::Result<SubsceneError>>(std::move(subscene));
}
