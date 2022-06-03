// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/mesh.h>
#include <asset/material.h>
#include <lib/math/vector.h>
#include <lib/math/quaternion.h>
#include <lib/math/color.h>
#include <lib/expected.h>
#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class JSON_SubSceneNodeType {
    empty,
    mesh,
    point_light,
    spot_light,
    directional_light,
    camera
};

JSON5_ENUM(JSON_SubSceneNodeType, empty, mesh, point_light, spot_light, directional_light, camera)

struct JSON_SubSceneNodeMeshDefinition {
    std::string mesh;
    std::vector<std::string> materials;
};

JSON5_CLASS(JSON_SubSceneNodeMeshDefinition, mesh, materials)

struct JSON_SubSceneNodePointLightDefinition {
    std::array<float, 3> color;
    float range;
};

JSON5_CLASS(JSON_SubSceneNodePointLightDefinition, color, range)

struct JSON_SubSceneNodeValuesDefinition {
    std::optional<JSON_SubSceneNodeMeshDefinition> mesh;
    std::optional<JSON_SubSceneNodePointLightDefinition> point_light;
    std::array<float, 3> position;
    std::array<float, 3> rotation;
    std::array<float, 3> scale;
};

JSON5_CLASS(JSON_SubSceneNodeValuesDefinition, mesh, point_light, position, rotation, scale)

struct JSON_SubSceneNodeDefinition {
    std::string name;
    JSON_SubSceneNodeType type;
    JSON_SubSceneNodeValuesDefinition values;
};

JSON5_CLASS(JSON_SubSceneNodeDefinition, name, type, values)

struct JSON_SubSceneLinkDefinition {
    std::string name;
    std::vector<std::string> childrens;
};

JSON5_CLASS(JSON_SubSceneLinkDefinition, name, childrens)

struct JSON_SubSceneDefinition {
    std::string name;
    std::vector<JSON_SubSceneNodeDefinition> nodes;
    std::vector<JSON_SubSceneLinkDefinition> links;
};

JSON5_CLASS(JSON_SubSceneDefinition, name, nodes, links)

namespace ionengine::asset {

class AssetManager;

enum class SubsceneError {
    IO,
    ParseError
};

enum class SubsceneNodeType {
    Empty,
    Mesh,
    PointLight,
    SpotLight,
    DirectionalLight,
    Camera
};

template<SubsceneNodeType Type>
struct SubsceneNodeData {};

template<>
struct SubsceneNodeData<SubsceneNodeType::Empty> {
    lib::math::Vector3f position;
    lib::math::Quaternionf rotation;
    lib::math::Vector3f scale;
};

template<>
struct SubsceneNodeData<SubsceneNodeType::Mesh> {
    asset::AssetPtr<asset::Mesh> mesh;
    std::vector<asset::AssetPtr<asset::Material>> materials;
    lib::math::Vector3f position;
    lib::math::Quaternionf rotation;
    lib::math::Vector3f scale;
};

template<>
struct SubsceneNodeData<SubsceneNodeType::PointLight> {
    lib::math::Color color;
    float range;
    lib::math::Vector3f position;
    lib::math::Quaternionf rotation;
    lib::math::Vector3f scale;
};

struct SubsceneNode {
    std::string name;

    std::variant<
        SubsceneNodeData<SubsceneNodeType::Empty>,
        SubsceneNodeData<SubsceneNodeType::Mesh>,
        SubsceneNodeData<SubsceneNodeType::PointLight>
    > data;
};

struct Subscene {
    std::string name;
    std::vector<SubsceneNode> nodes;
    std::unordered_map<std::string, std::vector<std::string>> links;
    uint64_t hash;

    static lib::Expected<Subscene, lib::Result<SubsceneError>> load_from_file(std::filesystem::path const& file_path, asset::AssetManager& asset_manager);
};

}
