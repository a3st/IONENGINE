// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <editor/framework.h>
#include <scene/mesh_node.h>
#include <scene/camera_node.h>
#include <scene/point_light_node.h>
#include <lib/scope_profiler.h>

using namespace ionengine;
using namespace ionengine::framework;

Framework::Framework(asset::AssetManager& asset_manager, input::InputManager& input_manager, ui::UserInterface& ui) :
    _input_manager(&input_manager),
    _scene(scene::Scene::load_from_file("content/levels/city17.json5", asset_manager).value()) {

    auto camera_object_node = _scene.graph().add_node<scene::CameraNode>();
    camera_object_node->name("main_camera");
    camera_object_node->position(lib::math::Vector3f(0.0f, 1.0f, 3.0f));
    camera_object_node->render_target(_editor_rt);

    _camera_node = camera_object_node;

    _original_rotation = _camera_node->rotation();
}

scene::Scene& Framework::scene() {
    return _scene;
}
