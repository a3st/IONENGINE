// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/kids_room.h>
#include <scene/mesh_node.h>
#include <scene/camera_node.h>
#include <scene/point_light_node.h>

using namespace ionengine;
using namespace ionengine::framework;

KidsRoom::KidsRoom(asset::AssetManager& asset_manager, input::InputManager& input_manager) :
    _input_manager(&input_manager) {

    asset::AssetPtr<asset::Mesh> tv_mesh;
    asset::AssetPtr<asset::Mesh> chair_mesh;
    asset::AssetPtr<asset::Mesh> cabinet_mesh;
    asset::AssetPtr<asset::Mesh> cube_mesh;
    asset::AssetPtr<asset::Mesh> cabinet_002_mesh;
    asset::AssetPtr<asset::Mesh> bed_mesh;
    asset::AssetPtr<asset::Mesh> rocket_mesh;

    asset::AssetPtr<asset::Material> tv_default_material;
    asset::AssetPtr<asset::Material> tv_screen_default_material;
    asset::AssetPtr<asset::Material> chair_default_material;
    asset::AssetPtr<asset::Material> cabinet_default_material;
    asset::AssetPtr<asset::Material> cube_default_material;
    asset::AssetPtr<asset::Material> cabinet_002_default_material;
    asset::AssetPtr<asset::Material> bed_default_material;
    asset::AssetPtr<asset::Material> rocket_default_material;

    asset_manager.get_texture("engine/editor/bulb.dds").wait();

    // Initialize assets
    {
        tv_mesh = asset_manager.get_mesh("content/objects/tv.obj");
        chair_mesh = asset_manager.get_mesh("content/objects/chair.obj");
        cabinet_mesh = asset_manager.get_mesh("content/objects/cabinet.obj");
        cube_mesh = asset_manager.get_mesh("content/objects/cube.obj");
        cabinet_002_mesh = asset_manager.get_mesh("content/objects/cabinet_002.obj");
        bed_mesh = asset_manager.get_mesh("content/objects/bed.obj");
        rocket_mesh = asset_manager.get_mesh("content/objects/RocketLauncher.obj");

        tv_default_material = asset_manager.get_material("content/materials/tv_default.json5");
        tv_screen_default_material = asset_manager.get_material("content/materials/tv_screen_default.json5");
        chair_default_material = asset_manager.get_material("content/materials/chair_default.json5");
        cabinet_default_material = asset_manager.get_material("content/materials/cabinet_default.json5");
        cube_default_material = asset_manager.get_material("content/materials/cube_default.json5");
        cabinet_002_default_material = asset_manager.get_material("content/materials/cabinet_002_default.json5");
        bed_default_material = asset_manager.get_material("content/materials/bed_default.json5");
        rocket_default_material = asset_manager.get_material("content/materials/rocket.json5");

        tv_mesh.wait();
        chair_mesh.wait();
        cabinet_mesh.wait();
        cube_mesh.wait();
        cabinet_002_mesh.wait();
        bed_mesh.wait();
        rocket_mesh.wait();

        tv_default_material.wait();
        tv_screen_default_material.wait();
        chair_default_material.wait();
        cabinet_default_material.wait();
        cube_default_material.wait();
        cabinet_002_default_material.wait();
        bed_default_material.wait();
        rocket_default_material.wait();
    }

    // Initialize scene objects
    {
        auto root_node = _scene.graph().root();

        auto chair_object_node = _scene.graph().add_node<scene::MeshNode>();
        chair_object_node->name("chair_object");
        chair_object_node->mesh(chair_mesh);
        chair_object_node->material(0, chair_default_material);
        chair_object_node->position(lib::math::Vector3f(0.0f, -1.0f, 3.0f));
        chair_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        chair_object_node->scale(lib::math::Vector3f(0.6f, 0.6f, 0.6f));

        auto chair_2_object_node = _scene.graph().add_node<scene::MeshNode>();
        chair_2_object_node->name("chair_2_object");
        chair_2_object_node->mesh(chair_mesh);
        chair_2_object_node->material(0, chair_default_material);
        chair_2_object_node->position(lib::math::Vector3f(-1.0f, -1.0f, 2.8f));
        chair_2_object_node->rotation(lib::math::Quaternionf::angle_axis(25.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        chair_2_object_node->scale(lib::math::Vector3f(0.6f, 0.6f, 0.6f));

        auto chair_3_object_node = _scene.graph().add_node<scene::MeshNode>();
        chair_3_object_node->name("chair_3_object");
        chair_3_object_node->mesh(chair_mesh);
        chair_3_object_node->material(0, chair_default_material);
        chair_3_object_node->position(lib::math::Vector3f(1.0f, -1.0f, 2.8f));
        chair_3_object_node->rotation(lib::math::Quaternionf::angle_axis(-25.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        chair_3_object_node->scale(lib::math::Vector3f(0.6f, 0.6f, 0.6f));

        auto bed_object_node = _scene.graph().add_node<scene::MeshNode>();
        bed_object_node->name("bed_object");
        bed_object_node->mesh(bed_mesh);
        bed_object_node->material(0, bed_default_material);
        bed_object_node->position(lib::math::Vector3f(-1.0f, -1.0f, 5.0f));
        bed_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        bed_object_node->scale(lib::math::Vector3f(0.6f, 0.6f, 0.6f));

        auto cabinet_002_object_node = _scene.graph().add_node<scene::MeshNode>();
        cabinet_002_object_node->name("cabinet_object");
        cabinet_002_object_node->mesh(cabinet_002_mesh);
        cabinet_002_object_node->material(0, cabinet_002_default_material);
        cabinet_002_object_node->position(lib::math::Vector3f(0.0f, -1.0f, 0.5f));
        cabinet_002_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        cabinet_002_object_node->scale(lib::math::Vector3f(0.5f, 0.5f, 0.5f));

        auto tv_object_node = _scene.graph().add_node<scene::MeshNode>();
        tv_object_node->name("tv_object");
        tv_object_node->mesh(tv_mesh);
        tv_object_node->material(0, tv_default_material);
        tv_object_node->material(1, tv_screen_default_material);
        tv_object_node->position(lib::math::Vector3f(0.0f, -0.25f, 0.8f));
        tv_object_node->rotation(lib::math::Quaternionf::angle_axis(0.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        tv_object_node->scale(lib::math::Vector3f(0.7f, 0.7f, 0.7f));

        auto cabinet_object_node = _scene.graph().add_node<scene::MeshNode>();
        cabinet_object_node->name("cabinet_object");
        cabinet_object_node->mesh(cabinet_mesh);
        cabinet_object_node->material(0, cabinet_default_material);
        cabinet_object_node->position(lib::math::Vector3f(3.0f, -1.0f, 5.5f));
        cabinet_object_node->rotation(lib::math::Quaternionf::angle_axis(90.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        cabinet_object_node->scale(lib::math::Vector3f(0.5f, 0.5f, 0.5f));

        auto rocket_object_node = _scene.graph().add_node<scene::MeshNode>();
        rocket_object_node->name("rocket_object");
        rocket_object_node->mesh(rocket_mesh);
        rocket_object_node->material(0, rocket_default_material);
        rocket_object_node->position(lib::math::Vector3f(0.0f, 4.0f, 5.5f));
        rocket_object_node->rotation(lib::math::Quaternionf::angle_axis(90.0f, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
        rocket_object_node->scale(lib::math::Vector3f(0.5f, 0.5f, 0.5f));

        std::array<scene::MeshNode*, 3> cube_object_nodes;

        for(size_t i = 0; i < cube_object_nodes.size(); ++i) {
            cube_object_nodes[i] = _scene.graph().add_node<scene::MeshNode>();
            cube_object_nodes[i]->name("cube_object_" + i);
            cube_object_nodes[i]->mesh(cube_mesh);
            cube_object_nodes[i]->material(0, cube_default_material);
            cube_object_nodes[i]->rotation(lib::math::Quaternionf::angle_axis(90.0f * i, lib::math::Vector3f(0.0f, 1.0f, 0.0f)));
            cube_object_nodes[i]->scale(lib::math::Vector3f(0.15f, 0.15f, 0.15f));
        }

        cube_object_nodes[0]->position(lib::math::Vector3f(0.0f, -1.0f, 4.5f));
        cube_object_nodes[1]->position(lib::math::Vector3f(0.6f, -1.0f, 4.5f));
        cube_object_nodes[2]->position(lib::math::Vector3f(0.4f, -1.0f, 5.0f));

        auto camera_object_node = _scene.graph().add_node<scene::CameraNode>();
        camera_object_node->name("MainCamera");
        camera_object_node->position(lib::math::Vector3f(0.0f, 1.0f, 3.0f));

        auto point_light_001_node = _scene.graph().add_node<scene::PointLightNode>();
        point_light_001_node->name("point_light_001");
        point_light_001_node->light_color(lib::math::Color(0.7f, 0.2f, 0.6f, 1.0f));
        point_light_001_node->position(lib::math::Vector3f(1.5f, 4.0f, 5.0f));
        point_light_001_node->editor_icon(asset_manager.get_texture("engine/editor/bulb.dds"));

        // Child objects to root scene node
        root_node->add_child(tv_object_node);
        root_node->add_child(cabinet_002_object_node);
        root_node->add_child(chair_object_node);
        root_node->add_child(chair_2_object_node);
        root_node->add_child(chair_3_object_node);
        root_node->add_child(cabinet_object_node);
        root_node->add_child(camera_object_node);
        root_node->add_child(bed_object_node);
        root_node->add_child(rocket_object_node);
        for(size_t i = 0; i < cube_object_nodes.size(); ++i) {
            root_node->add_child(cube_object_nodes[i]);
        }

        _camera_node = camera_object_node;

        _original_rotation = _camera_node->rotation();
    }
}

void KidsRoom::update(float const delta_time) {

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

scene::Scene& KidsRoom::scene() {
    return _scene;
}
