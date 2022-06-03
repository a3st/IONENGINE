// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/subscene.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::asset;

lib::Expected<Subscene, lib::Result<SubsceneError>> Subscene::load_from_file(std::filesystem::path const& file_path, asset::AssetManager& asset_manager) {

    std::string path_string = file_path.string();

    JSON_SubSceneDefinition document;
    json5::error result = json5::from_file(path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::Expected<Subscene, lib::Result<SubsceneError>>::error(
            lib::Result<SubsceneError> { .errc = SubsceneError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::Expected<Subscene, lib::Result<SubsceneError>>::error(
            lib::Result<SubsceneError> { .errc = SubsceneError::ParseError, .message = "Parse file error" }
        );
    }

    auto subscene = Subscene {};
    subscene.name = document.name;
    
    for(auto const& node : document.nodes) {

        subscene.nodes.emplace_back();
        subscene.nodes.back().name = node.name;

        auto position = lib::math::Vector3f(node.values.position.at(0), node.values.position.at(1), node.values.position.at(2));

        auto quat_x = lib::math::Quaternionf::angle_axis(node.values.rotation.at(0), lib::math::Vector3f(1.0f, 0.0f, 0.0f));
        auto quat_y = lib::math::Quaternionf::angle_axis(node.values.rotation.at(1), lib::math::Vector3f(0.0f, 1.0f, 0.0f));
        auto quat_z = lib::math::Quaternionf::angle_axis(node.values.rotation.at(2), lib::math::Vector3f(0.0f, 0.0f, 1.0f));
        auto quat_final = quat_x * quat_y * quat_z;

        auto scale = lib::math::Vector3f(node.values.scale.at(0), node.values.scale.at(1), node.values.scale.at(2));

        switch(node.type) {

            case JSON_SubSceneNodeType::empty: {

                auto data = SubsceneNodeData<SubsceneNodeType::Empty> { 
                    .position = position,
                    .rotation = quat_final,
                    .scale = scale
                };

                subscene.nodes.back().data = std::move(data);

            } break;

            case JSON_SubSceneNodeType::mesh: {

                auto& mesh_node_data = node.values.mesh.value();

                asset::AssetPtr<asset::Mesh> mesh = asset_manager.get_mesh(mesh_node_data.mesh);

                std::vector<asset::AssetPtr<asset::Material>> materials;
                for(auto const& material : mesh_node_data.materials) {
                    materials.push_back(asset_manager.get_material(material));
                }

                auto data = SubsceneNodeData<SubsceneNodeType::Mesh> {
                    .mesh = std::move(mesh),
                    .materials = std::move(materials),
                    .position = std::move(position),
                    .rotation = std::move(quat_final),
                    .scale = std::move(scale)
                };

                subscene.nodes.back().data = std::move(data);

            } break;

            case JSON_SubSceneNodeType::point_light: {

                auto& point_light_data = node.values.point_light.value();

                auto color = lib::math::Color(point_light_data.color.at(0), point_light_data.color.at(1), point_light_data.color.at(2), 1.0f);

                auto data = SubsceneNodeData<SubsceneNodeType::PointLight> {
                    .color = std::move(color),
                    .range = point_light_data.range,
                    .position = std::move(position),
                    .rotation = std::move(quat_final),
                    .scale = std::move(scale)
                };

                subscene.nodes.back().data = std::move(data);
            } break;
        }
    }
    
    for(auto const& link : document.links) {
        
        std::vector<std::string> childrens;
        
        for(auto const& children : link.childrens) {
            childrens.emplace_back(children);
        }

        subscene.links.insert({ link.name, std::move(childrens) });
    }

    return lib::Expected<Subscene, lib::Result<SubsceneError>>::ok(std::move(subscene));
}
