// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/ui_renderer.h>
#include <renderer/shader_binder.h>
#include <ui/user_interface.h>

using namespace ionengine;
using namespace ionengine::renderer;

UiRenderer::UiRenderer(backend::Device& device, UploadManager& upload_manager, std::vector<RTTextureCache>& rt_texture_caches, platform::Window& window, asset::AssetManager& asset_manager) :
    _device(&device),
    _asset_manager(&asset_manager),
    _upload_manager(&upload_manager),
    _rt_texture_caches(&rt_texture_caches),
    _texture_cache(device),
    _width(window.client_width()),
    _height(window.client_height()) {
    
    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _ui_element_pools.emplace_back(*_device, 512, BufferPoolUsage::Dynamic);
        _geometry_pools.emplace_back(*_device, 512, GeometryPoolUsage::Dynamic);
    }
}

UiRenderer::~UiRenderer() {

}

void UiRenderer::update(float const delta_time) {

}

void UiRenderer::resize(uint32_t const width, uint32_t const height) {
    
}

void UiRenderer::render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, NullData& null, FrameGraph& frame_graph, ui::UserInterface& ui, uint32_t const frame_index) {
    _ui_element_pools.at(frame_index).reset();
    _geometry_pools.at(frame_index).reset();
    
    auto swapchain_color_info = CreateColorInfo {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Load,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };
    
    frame_graph.add_pass(
        "ui",
        _width,
        _height,
        std::span<CreateColorInfo const>(&swapchain_color_info, 1),
        std::nullopt,
        std::nullopt,
        TaskExecution::Single,
        [=, &pipeline_cache, &shader_cache, &ui, &null](RenderPassContext const& context) {

            lib::ObjectPtr<Shader> shader = shader_cache.get("ui_pc");
            auto pipeline = pipeline_cache.get(*shader, context.render_pass);

            _device->bind_pipeline(context.command_list, pipeline);

            ShaderBinder binder(*shader, null);

            ui.render_interface()._ui_element_pool = &_ui_element_pools.at(frame_index);
            ui.render_interface()._geometry_pool = &_geometry_pools.at(frame_index);
            ui.render_interface()._rt_texture_cache = &_rt_texture_caches->at(frame_index);
            ui.render_interface()._command_list = context.command_list;
            ui.render_interface()._width = _width;
            ui.render_interface()._height = _height;
            ui.render_interface()._binder = &binder;
            ui.render_interface()._shader = &*shader;

            ui.context().Render();
        }
    );
}
