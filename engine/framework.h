// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_manager.h>
#include <input/input_manager.h>
#include <scene/scene.h>
#include <scene/camera_node.h>

namespace ionengine::framework {

class Framework {
public:

    Framework(asset::AssetManager& asset_manager, input::InputManager& input_manager);

    void update(float const delta_time);

    scene::Scene& scene();

private:

    input::InputManager* _input_manager;

    scene::CameraNode* _camera_node;

    float _rotation_x = 0.0f;
    float _rotation_y = 0.0f;

    lib::math::Quaternionf _original_rotation;

    scene::Scene _scene;
};

}