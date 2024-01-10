// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/framework.h>
#include <scene/mesh_node.h>
#include <scene/camera_node.h>
#include <scene/point_light_node.h>
#include <scene/world_environment_node.h>

using namespace ionengine;
using namespace ionengine::framework;

Framework::Framework(asset::AssetManager& asset_manager, input::InputManager& input_manager, scene::Scene& scene, ui::UserInterface& ui) :
    _input_manager(&input_manager) {

    scene.load("content/levels/city17.subscene");
    scene.load("content/levels/city17_2.subscene");

    /*scene.find_by_name<scene::TransformNode>("city17_root")->name("city17_root (1)");

    scene.load("content/levels/city17.subscene");
    auto sub_root = scene.find_by_name<scene::TransformNode>("city17_root"); 
    sub_root->name("city17_root (2)");
    sub_root->position(lib::math::Vector3f(15.0f, 0.0f, 0.0f));

    scene.load("content/levels/city17.subscene");
    sub_root = scene.find_by_name<scene::TransformNode>("city17_root"); 
    sub_root->name("city17_root (3)");
    sub_root->position(lib::math::Vector3f(-15.0f, 0.0f, 15.0f));*/

    auto camera_object_node = scene.add_node<scene::CameraNode>();
    camera_object_node->name("main_camera");
    camera_object_node->position(lib::math::Vector3f(0.0f, 0.0f, 0.0f));

    scene.graph().root()->add_child(camera_object_node);

    auto world_environment_node = scene.add_node<scene::WorldEnvironmentNode>();
    world_environment_node->name("world_environment");
    world_environment_node->skybox_material(asset_manager.get_material("content/materials/skyboxes/sky_01.mat"));

    scene.graph().root()->add_child(world_environment_node);

    _camera_node = camera_object_node;

    _original_rotation = _camera_node->rotation();
}

void Framework::update(float const delta_time) {

    if(_input_manager->key(input::KeyCode::S)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f fwd_direction = _camera_node->rotation() * lib::math::Vector3f(0.0f, 0.0f, 1.0f);
        _camera_node->position(position + fwd_direction * delta_time * 3.0f);
    }

    if(_input_manager->key(input::KeyCode::W)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f fwd_direction = _camera_node->rotation() * lib::math::Vector3f(0.0f, 0.0f, -1.0f);
        _camera_node->position(position + fwd_direction * delta_time * 3.0f);
    }

    if(_input_manager->key(input::KeyCode::D)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f right_direction = _camera_node->rotation() * lib::math::Vector3f(1.0f, 0.0f, 0.0f);
        _camera_node->position(position + right_direction * delta_time * 3.0f);
    }

    if(_input_manager->key(input::KeyCode::A)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f right_direction = _camera_node->rotation() * lib::math::Vector3f(-1.0f, 0.0f, 0.0f);
        _camera_node->position(position + right_direction * delta_time * 3.0f);
    }

    float mouse_x_value = _input_manager->axis(input::AxisCode::MouseX);
    float mouse_y_value = _input_manager->axis(input::AxisCode::MouseY);

    _rotation_x += -mouse_x_value * 25.0f * delta_time;
    _rotation_y += -mouse_y_value * 25.0f * delta_time;

    lib::math::Quaternionf quat_x = lib::math::Quaternionf::angle_axis(_rotation_x, lib::math::Vector3f(0.0f, 1.0f, 0.0f));
    lib::math::Quaternionf quat_y = lib::math::Quaternionf::angle_axis(_rotation_y, lib::math::Vector3f(1.0f, 0.0f, 0.0f));

    _camera_node->rotation(_original_rotation * quat_x * quat_y);
}
