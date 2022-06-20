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

Renderer::Renderer(platform::Window& window, asset::AssetManager& asset_manager) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = backend::BACKEND_BACK_BUFFER_COUNT }),
    _upload_manager(_device),
    _frame_graph(_device),
    _shader_cache(_device),
    _pipeline_cache(_device),
    _ui_renderer(_device, _upload_manager, window, asset_manager),
    _mesh_renderer(_device, _upload_manager, window, asset_manager),
    _width(window.client_width()),
    _height(window.client_height()) {

    _null = NullData {
        .cbuffer = make_resource_ptr(GPUBuffer::create(_device, 256, backend::BufferFlags::ConstantBuffer | backend::BufferFlags::HostWrite).as_ok()),
        .sbuffer = make_resource_ptr(GPUBuffer::create(_device, 256, backend::BufferFlags::ShaderResource | backend::BufferFlags::HostWrite, 256).as_ok()),
        .texture = make_resource_ptr(GPUTexture::create(_device, backend::Format::RGBA8_UNORM, 1, 1, 1, 1, backend::TextureFlags::ShaderResource).as_ok())
    };

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _rt_texture_caches.emplace_back(_device);

        auto gpu_texture = GPUTexture {
            .texture = _device.swapchain_texture(i),
            .memory_state = backend::MemoryState::Common,
            .is_swapchain = true
        };
        _swap_textures.emplace_back(make_resource_ptr(std::move(gpu_texture)));
    }

    asset_manager.get_shader("engine/shaders/gbuffer.shader")->wait();
    asset_manager.get_shader("engine/shaders/forward.shader")->wait();
}

Renderer::~Renderer() {
    _frame_graph.reset();
}

void Renderer::update(float const delta_time) {
    SCOPE_PROFILE()

    _mesh_renderer.update(delta_time, _upload_batches);
}

void Renderer::render(scene::Scene& scene, ui::UserInterface& ui) {
    SCOPE_PROFILE()

    bool is_skip_upload = false;

    if(!_upload_batches.empty() && !is_skip_upload) {
        auto& batch = _upload_batches.front();

        bool result = false;

        auto batch_data_visitor = make_visitor(
            [&](UploadBatchData<GeometryBuffer>& data) {
                result = _upload_manager.upload_geometry_data(
                    data.geometry_buffer, 
                    data.asset->get().surfaces()[data.surface_index].vertices.to_span(),
                    data.asset->get().surfaces()[data.surface_index].indices.to_span()
                );
            },
            [&](UploadBatchData<GPUTexture>& data) {
                result = _upload_manager.upload_texture_data(
                    data.texture, 
                    data.asset->get().data.to_span()
                );
            }
        );

        std::visit(batch_data_visitor, batch.data);

        if(result) {
            _upload_batches.pop();
        } else {
            is_skip_upload = true;
        }
    }

    _upload_manager.dispatch();

    uint32_t const frame_index = _frame_graph.wait();

    _mesh_renderer.render(_pipeline_cache, _shader_cache, _rt_texture_caches.at(frame_index), _null, _frame_graph, scene, _swap_textures.at(frame_index), frame_index);
    _ui_renderer.render(_pipeline_cache, _shader_cache, _rt_texture_caches.at(frame_index), _null, _frame_graph, ui, _swap_textures.at(frame_index), frame_index);
    
    _frame_graph.add_present_pass(_swap_textures.at(frame_index));

    _frame_graph.execute();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {
    if(_width != width || _height != height) {

        _frame_graph.reset();
        
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
