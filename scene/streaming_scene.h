// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <asset/subscene.h>
#include <lib/expected.h>

namespace ionengine::scene {

enum class StreamingSceneError {};

struct StreamingScene {
    std::vector<std::unique_ptr<SceneNode>> nodes;

    static lib::Expected<StreamingScene, lib::Result<StreamingSceneError>> load_from_subscene(asset::Subscene const& subscene);
};

}
