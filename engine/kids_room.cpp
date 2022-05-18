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

    asset::AssetPtr<asset::Material> tv_default_material;
    asset::AssetPtr<asset::Material> tv_screen_default_material;
    asset::AssetPtr<asset::Material> chair_default_material;

    // Initialize assets
    {
        tv_mesh = asset_manager.get_mesh("content/objects/tv.obj");
        chair_mesh = asset_manager.get_mesh("content/objects/chair.obj");

        tv_default_material = asset_manager.get_material("content/materials/tv_default.json5");
        tv_screen_default_material = asset_manager.get_material("content/materials/tv_screen_default.json5");
        chair_default_material = asset_manager.get_material("content/materials/chair_default.json5");

        tv_mesh.wait();
        chair_mesh.wait();

        tv_default_material.wait();
        tv_screen_default_material.wait();
        chair_default_material.wait();
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
        tv_object_node->position(lib::math::Vector3f(0.0f, -0.5f, 0.5f));
        tv_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        tv_object_node->scale(lib::math::Vector3f(1.0f, 1.0f, 1.0f));

        auto camera_object_node = _scene.graph().add_node<scene::CameraNode>();
        camera_object_node->name("MainCamera");
        camera_object_node->position(lib::math::Vector3f(0.0f, 2.0f, 3.0f));
        

        // Child objects to root scene node
        root_node->add_child(tv_object_node);
        root_node->add_child(chair_object_node);
        root_node->add_child(camera_object_node);

        _camera_node = camera_object_node;
    }
}

void KidsRoom::update(float const delta_time) {

    if(_input_manager->key(input::KeyCode::S)) {
        lib::math::Vector3f position = _camera_node->position();
        _camera_node->position(lib::math::Vector3f(position.x, position.y, position.z + 1.0f * delta_time));
    }

    if(_input_manager->key(input::KeyCode::W)) {
        lib::math::Vector3f position = _camera_node->position();
        _camera_node->position(lib::math::Vector3f(position.x, position.y, position.z - 1.0f * delta_time));
    }

    if(_input_manager->key(input::KeyCode::D)) {
        lib::math::Vector3f position = _camera_node->position();
        _camera_node->position(lib::math::Vector3f(position.x + 1.0f * delta_time, position.y, position.z));
    }

    if(_input_manager->key(input::KeyCode::A)) {
        lib::math::Vector3f position = _camera_node->position();
        _camera_node->position(lib::math::Vector3f(position.x - 1.0f * delta_time, position.y, position.z));
    }

    if(_input_manager->key(input::KeyCode::Space)) {
        lib::math::Vector3f position = _camera_node->position();
        _camera_node->position(lib::math::Vector3f(position.x, position.y - 1.0f * delta_time, position.z));
    }

    if(_input_manager->key(input::KeyCode::V)) {
        lib::math::Vector3f position = _camera_node->position();
        _camera_node->position(lib::math::Vector3f(position.x, position.y + 1.0f * delta_time, position.z));
    }
}

scene::Scene& KidsRoom::scene() {
    return _scene;
}
