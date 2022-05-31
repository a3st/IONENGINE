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

    _null = NullData {
        .cbuffer = ResourcePtr<GPUBuffer>(GPUBuffer::create(_device, 256, backend::BufferFlags::ConstantBuffer | backend::BufferFlags::HostWrite).value()),
        .sbuffer = ResourcePtr<GPUBuffer>(GPUBuffer::create(_device, 256, backend::BufferFlags::ShaderResource | backend::BufferFlags::HostWrite, 256).value()),
        .texture = ResourcePtr<GPUTexture>(GPUTexture::create(_device, backend::Format::RGBA8_UNORM, 1, 1, 1, 1, backend::TextureFlags::ShaderResource).value())
    };
}

Renderer::~Renderer() {
    _frame_graph.reset();
}

void Renderer::update(float const delta_time) {
    SCOPE_PROFILE()
    
    _upload_manager.update();
}

void Renderer::render(scene::Scene& scene, ui::UserInterface& ui) {
    SCOPE_PROFILE()

    uint32_t const frame_index = _frame_graph.wait();

    _mesh_renderer.render(_pipeline_cache, _shader_cache, _null, _frame_graph, scene, frame_index);
    _ui_renderer.render(_pipeline_cache, _shader_cache, _null, _frame_graph, ui, frame_index);
    
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
