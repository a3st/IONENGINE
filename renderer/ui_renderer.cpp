// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/ui_renderer.h>
#include <ui/user_interface.h>

using namespace ionengine;
using namespace ionengine::renderer;

UiRenderer::UiRenderer(backend::Device& device, UploadManager& upload_manager, platform::Window& window, asset::AssetManager& asset_manager) :
    _device(&device),
    _asset_manager(&asset_manager),
    _upload_manager(&upload_manager),
    _texture_cache(device),
    _width(window.client_width()),
    _height(window.client_height()),
    _ui_technique(asset_manager.get_technique(UI_TECHNIQUE_PATH)) {
    
    _ui_technique.wait();
}

UiRenderer::~UiRenderer() {

}

void UiRenderer::update(float const delta_time) {

}

void UiRenderer::resize(uint32_t const width, uint32_t const height) {

}

void UiRenderer::render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, FrameGraph& frame_graph, ui::UserInterface& ui, uint32_t const frame_index) {
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
        [=](RenderPassContext const& context) {

            /*auto [pipeline, shader_program] = _pipeline_cache.get(_shader_cache, *_ui_technique, context.render_pass);

            _device.bind_pipeline(context.command_list, pipeline);

            ShaderUniformBinder binder(_device, *shader_program);

            _ui_renderer.apply_command_list(context.command_list, binder, *shader_program, frame_index);
            ui.context().Render();*/
        }
    );
}
