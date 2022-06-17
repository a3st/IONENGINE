// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/ui_renderer.h>
#include <renderer/descriptor_binder.h>
#include <ui/user_interface.h>

using namespace ionengine;
using namespace ionengine::renderer;

UiRenderer::UiRenderer(backend::Device& device, UploadManager& upload_manager, platform::Window& window, asset::AssetManager& asset_manager) :
    _device(&device),
    _asset_manager(&asset_manager),
    _upload_manager(&upload_manager),
    _texture_cache(device),
    _width(window.client_width()),
    _height(window.client_height()) {
    
    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _ui_element_pools.emplace_back(*_device, 512, BufferPoolUsage::Dynamic);
        _geometry_pools.emplace_back(*_device, 512, GeometryPoolUsage::Dynamic);
    }

    _ui_shader = asset_manager.get_shader("engine/shaders/ui.shader");
    _ui_shader->wait();
}

UiRenderer::~UiRenderer() {

}

void UiRenderer::update(float const delta_time) {

}

void UiRenderer::resize(uint32_t const width, uint32_t const height) {
    
}

void UiRenderer::render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, RTTextureCache& rt_texture_cache, NullData& null, FrameGraph& frame_graph, ui::UserInterface& ui, ResourcePtr<GPUTexture> swap_texture, uint32_t const frame_index) {
    _ui_element_pools.at(frame_index).reset();
    _geometry_pools.at(frame_index).reset();
    
    auto swapchain_color_info = CreateColorInfo {
        .attachment = swap_texture,
        .load_op = backend::RenderPassLoadOp::Load,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };
    
    frame_graph.add_pass(
        "ui",
        _width,
        _height,
        std::span<CreateColorInfo const>(&swapchain_color_info, 1),
        std::span<CreateInputInfo const>(),
        std::nullopt,
        [&, frame_index](RenderPassContext& context) -> PassTaskResult {

            ResourcePtr<CommandList> command_list = context.command_pool->allocate();

            ResourcePtr<GPUProgram> program = shader_cache.get(_ui_shader->get());
            ResourcePtr<GPUPipeline> gpu_pipeline = pipeline_cache.get(program->get(), _ui_shader->get().draw_parameters, context.render_pass->get());

            gpu_pipeline->get().bind(*_device, command_list->get());

            DescriptorBinder binder(program, null);

            ui.render_interface()._ui_element_pool = &_ui_element_pools.at(frame_index);
            ui.render_interface()._geometry_pool = &_geometry_pools.at(frame_index);
            ui.render_interface()._rt_texture_cache = &rt_texture_cache;
            ui.render_interface()._command_list = command_list;
            ui.render_interface()._width = _width;
            ui.render_interface()._height = _height;
            ui.render_interface()._binder = &binder;
            ui.render_interface()._program = program;

            ui.context().Render();

            ui.render_interface()._command_list = nullptr;
            ui.render_interface()._program = nullptr;
            ui.render_interface()._binder = nullptr;

            command_list->get().close(*_device);

            return PassTaskResult::singlethread(std::move(command_list)); 
        }
    );
}
