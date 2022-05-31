// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <ui/user_interface.h>
#include <scene/scene_visitor.h>
#include <scene/mesh_node.h>
#include <scene/transform_node.h>
#include <scene/camera_node.h>
#include <scene/point_light_node.h>
#include <lib/scope_profiler.h>

using namespace ionengine;
using namespace ionengine::renderer;

Renderer::Renderer(platform::Window& window, asset::AssetManager& asset_manager, lib::ThreadPool& thread_pool) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = 2 }),
    _upload_manager(thread_pool, _device),
    _frame_graph(_device),
    _ui_renderer(_device, _upload_manager, window, asset_manager),
    _mesh_renderer(_device, _upload_manager, window, asset_manager),
    _shader_cache(_device),
    _pipeline_cache(_device),
    _width(window.client_width()),
    _height(window.client_height()) {

    /*
    auto [mesh_sender, mesh_receiver] = lib::make_channel<asset::AssetEvent<asset::Mesh>>();
    asset_manager.mesh_pool().event_dispatcher().add(std::move(mesh_sender));
    _mesh_event_receiver.emplace(std::move(mesh_receiver));

    auto [technique_sender, technique_receiver] = lib::make_channel<asset::AssetEvent<asset::Technique>>();
    asset_manager.technique_pool().event_dispatcher().add(std::move(technique_sender));
    _technique_event_receiver.emplace(std::move(technique_receiver));

    auto [texture_sender, texture_receiver] = lib::make_channel<asset::AssetEvent<asset::Texture>>();
    asset_manager.texture_pool().event_dispatcher().add(std::move(texture_sender));
    _texture_event_receiver.emplace(std::move(texture_receiver)); 
    */

    /*
    

    recreate_gbuffer(_width, _height);

    for(uint32_t i = 0; i < 2; ++i) {
        
    }
    */
}

Renderer::~Renderer() {
    _frame_graph.reset();
}

void Renderer::update(float const delta_time) {
    SCOPE_PROFILE()
    /*
    {
        asset::AssetEvent<asset::Mesh> mesh_event;
        while(_mesh_event_receiver.value().try_receive(mesh_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Loaded>& event) {
                    for(size_t i = 0; i < event.asset->surfaces().size(); ++i) {
                        //_geometry_cache.get(_upload_manager, *event.asset, static_cast<uint32_t>(i));
                    }
                },
                // Default
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, mesh_event.data);
        }
    }
    */
}

void Renderer::render(scene::Scene& scene, ui::UserInterface& ui) {
    SCOPE_PROFILE()

    _upload_manager.update();

    uint32_t const frame_index = _frame_graph.wait();
    
    /*scene::CameraNode* camera = scene.graph().find_by_name<scene::CameraNode>("MainCamera");

    _opaque_queue.clear();
    _transculent_queue.clear();
    _point_lights.clear();
    _editor_instances.clear();

    _world_pools.at(frame_index).reset();
    _object_pools.at(frame_index).reset();
    _point_light_pools.at(frame_index).reset();
    _ui_renderer.reset(frame_index);

    MeshVisitor mesh_visitor(_opaque_queue, _transculent_queue, _point_lights, _editor_instances);
    scene.graph().visit(scene.graph().begin(), scene.graph().end(), mesh_visitor);

    camera->calculate_matrices();
    _opaque_queue.sort();
    _transculent_queue.sort();*/
    
    _frame_graph.execute();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

    if(_width != width || _height != height) {

        _frame_graph.reset();

        _device.recreate_swapchain(width, height);
        _width = width;
        _height = height;
    }
}

UiRenderer& Renderer::ui_renderer() {
    return _ui_renderer;
}

MeshRenderer& Renderer::mesh_renderer() {
    return _mesh_renderer;
}

/*

ResourcePtr<GeometryBuffer> Renderer::create_quad(backend::Device& device, UploadManager& upload_manager) {

    auto result = GeometryBuffer::procedural(device, 20, 6);
    if(result.is_ok()) {

        auto geometry_buffer = ResourcePtr<GeometryBuffer>(std::move(result.value()));
        
        auto vertex_data = std::vector<float> {
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f
        };

        auto index_data = std::vector<uint32_t> { 0, 1, 2, 0, 3, 1 };

        upload_manager.upload_geometry_data(
            geometry_buffer, 
            std::span<uint8_t const>((uint8_t const*)vertex_data.data(), vertex_data.size() * sizeof(float)),
            std::span<uint8_t const>((uint8_t const*)index_data.data(), index_data.size() * sizeof(uint32_t))
        );

        return geometry_buffer;

    } else {
        throw lib::Exception(result.error_value().message);
    }
}
*/