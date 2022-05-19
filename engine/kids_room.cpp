// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/kids_room.h>
#include <scene/mesh_node.h>
#include <scene/camera_node.h>

using namespace ionengine;
using namespace ionengine::framework;

KidsRoom::KidsRoom(asset::AssetManager& asset_manager, input::InputManager& input_manager) :
    _input_manager(&input_manager) {

    asset::AssetPtr<asset::Mesh> tv_mesh;
    asset::AssetPtr<asset::Mesh> chair_mesh;
    asset::AssetPtr<asset::Mesh> cabinet_mesh;

    asset::AssetPtr<asset::Material> tv_default_material;
    asset::AssetPtr<asset::Material> tv_screen_default_material;
    asset::AssetPtr<asset::Material> chair_default_material;
    asset::AssetPtr<asset::Material> cabinet_default_material;

    // Initialize assets
    {
        tv_mesh = asset_manager.get_mesh("content/objects/tv.obj");
        chair_mesh = asset_manager.get_mesh("content/objects/chair.obj");
        cabinet_mesh = asset_manager.get_mesh("content/objects/cabinet.obj");

        tv_default_material = asset_manager.get_material("content/materials/tv_default.json5");
        tv_screen_default_material = asset_manager.get_material("content/materials/tv_screen_default.json5");
        chair_default_material = asset_manager.get_material("content/materials/chair_default.json5");
        cabinet_default_material = asset_manager.get_material("content/materials/cabinet_default.json5");

        tv_mesh.wait();
        chair_mesh.wait();
        cabinet_mesh.wait();

        tv_default_material.wait();
        tv_screen_default_material.wait();
        chair_default_material.wait();
        cabinet_default_material.wait();
    }

    // Initialize scene objects
    {
        auto root_node = _scene.graph().root();

        auto chair_object_node = _scene.graph().add_node<scene::MeshNode>();
        chair_object_node->name("chair_object");
        chair_object_node->mesh(chair_mesh);
        chair_object_node->material(0, chair_default_material);
        chair_object_node->position(lib::math::Vector3f(0.0f, -1.0f, 0.5f));
        chair_object_node->rotation(lib::math::Quaternionf::angle_axis(175.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        chair_object_node->scale(lib::math::Vector3f(1.0f, 1.0f, 1.0f));

        auto tv_object_node = _scene.graph().add_node<scene::MeshNode>();
        tv_object_node->name("tv_object");
        tv_object_node->mesh(tv_mesh);
        tv_object_node->material(0, tv_default_material);
        tv_object_node->material(1, tv_screen_default_material);
        tv_object_node->position(lib::math::Vector3f(0.0f, -0.25f, 0.5f));
        tv_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        tv_object_node->scale(lib::math::Vector3f(1.0f, 1.0f, 1.0f));

        auto cabinet_object_node = _scene.graph().add_node<scene::MeshNode>();
        cabinet_object_node->name("cabinet_object");
        cabinet_object_node->mesh(cabinet_mesh);
        cabinet_object_node->material(0, cabinet_default_material);
        cabinet_object_node->position(lib::math::Vector3f(5.0f, -2.0f, 0.5f));
        cabinet_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        cabinet_object_node->scale(lib::math::Vector3f(0.5f, 0.5f, 0.5f));

        auto camera_object_node = _scene.graph().add_node<scene::CameraNode>();
        camera_object_node->name("MainCamera");
        camera_object_node->position(lib::math::Vector3f(0.0f, 2.0f, 3.0f));

        // Child objects to root scene node
        root_node->add_child(tv_object_node);
        root_node->add_child(chair_object_node);
        root_node->add_child(cabinet_object_node);
        root_node->add_child(camera_object_node);

        _camera_node = camera_object_node;

        _original_rotation = _camera_node->rotation();
    }
}

void KidsRoom::update(float const delta_time) {

    if(_input_manager->key(input::KeyCode::S)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f fwd_direction = _camera_node->rotation() * lib::math::Vector3f(0.0f, 0.0f, 1.0f);
        _camera_node->position(position + fwd_direction * delta_time);
    }

    if(_input_manager->key(input::KeyCode::W)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f fwd_direction = _camera_node->rotation() * lib::math::Vector3f(0.0f, 0.0f, -1.0f);
        _camera_node->position(position + fwd_direction * delta_time);
    }

    if(_input_manager->key(input::KeyCode::D)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f right_direction = _camera_node->rotation() * lib::math::Vector3f(1.0f, 0.0f, 0.0f);
        _camera_node->position(position + right_direction * delta_time);
    }

    if(_input_manager->key(input::KeyCode::A)) {
        lib::math::Vector3f position = _camera_node->position();
        lib::math::Vector3f right_direction = _camera_node->rotation() * lib::math::Vector3f(-1.0f, 0.0f, 0.0f);
        _camera_node->position(position + right_direction * delta_time);
    }

    float mouse_x_value = _input_manager->axis(input::AxisCode::MouseX);
    float mouse_y_value = _input_manager->axis(input::AxisCode::MouseY);

    _rotation_x += -mouse_x_value * 15.0f * delta_time;
    _rotation_y += -mouse_y_value * 15.0f * delta_time;

    lib::math::Quaternionf quat_x = lib::math::Quaternionf::angle_axis(_rotation_x, lib::math::Vector3f(0.0f, 1.0f, 0.0f));
    lib::math::Quaternionf quat_y = lib::math::Quaternionf::angle_axis(_rotation_y, lib::math::Vector3f(1.0f, 0.0f, 0.0f));

    _camera_node->rotation(_original_rotation * quat_x * quat_y);
}

scene::Scene& KidsRoom::scene() {
    return _scene;
}
