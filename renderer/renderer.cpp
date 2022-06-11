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
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = backend::BACKEND_BACK_BUFFER_COUNT }),
    _upload_manager(thread_pool, _device),
    _frame_graph(thread_pool, _device),
    _shader_cache(_device),
    _pipeline_cache(_device),
    _ui_renderer(_device, _upload_manager, _rt_texture_caches, window, asset_manager),
    _mesh_renderer(_device, _upload_manager, _rt_texture_caches, window, asset_manager),
    _width(window.client_width()),
    _height(window.client_height()) {

    _null = NullData {
        .cbuffer = make_resource_ptr(GPUBuffer::create(_device, 256, backend::BufferFlags::ConstantBuffer | backend::BufferFlags::HostWrite).as_ok()),
        .sbuffer = make_resource_ptr(GPUBuffer::create(_device, 256, backend::BufferFlags::ShaderResource | backend::BufferFlags::HostWrite, 256).as_ok()),
        .texture = make_resource_ptr(GPUTexture::create(_device, backend::Format::RGBA8_UNORM, 1, 1, 1, 1, backend::TextureFlags::ShaderResource).as_ok())
    };

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _rt_texture_caches.emplace_back(_device);
    }

    auto default_shader_paths = std::vector<std::filesystem::path> {
        "engine/shaders/deffered.shader",
        "engine/shaders/forward.shader",
        "engine/shaders/gbuffer.shader",
        "engine/shaders/ui.shader",
        "engine/shaders/fxaa.shader"
    };

    load_shaders(default_shader_paths);
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
        
        _width = width;
        _height = height;
    }
}

void Renderer::load_shaders(std::span<std::filesystem::path const> const shader_paths) {
    
}

UiRenderer& Renderer::ui_renderer() {
    return _ui_renderer;
}

MeshRenderer& Renderer::mesh_renderer() {
    return _mesh_renderer;
}
