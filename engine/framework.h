// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_manager.h>
#include <input/input_manager.h>
#include <scene/scene.h>
#include <ui/user_interface.h>
#include <scene/camera_node.h>
#include <lib/math/vector.h>

namespace ionengine::framework {

class Framework {
public:

    Framework(asset::AssetManager& asset_manager, input::InputManager& input_manager, scene::Scene& scene, ui::UserInterface& ui);

    void update(float const delta_time);

private:

    input::InputManager* _input_manager;

    scene::CameraNode* _camera_node;

    float _rotation_x = 0.0f;
    float _rotation_y = 0.0f;

    lib::math::Quaternionf _original_rotation;
};

}