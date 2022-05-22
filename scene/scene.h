// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <scene/scene_graph.h>
#include <lib/expected.h>
#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class JSON_SubSceneNodeType {
    mesh,
    point_light
};

JSON5_ENUM(JSON_SubSceneNodeType, mesh, point_light)

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
}

namespace ionengine::scene {

enum class SceneError {
    IO,
    ParseError
};

class Scene {
public:

    Scene();

    static lib::Expected<Scene, lib::Result<SceneError>> load_from_file(std::filesystem::path const& file_path, asset::AssetManager& asset_manager);

    SceneGraph& graph();

private:

    Scene(JSON_SubSceneDefinition const& document, asset::AssetManager& asset_manager);

    SceneGraph _scene_graph;
};

}
