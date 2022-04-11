// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>

using namespace ionengine::renderer;

Renderer::Renderer(platform::Window& window) 
    : 
        _context(window, 2)
    {

    build_frame_graph(window.client_size().width, window.client_size().height, 2);
}

void Renderer::render(scene::Scene& scene) {
    
    auto frame_texture = _context.get_or_wait_previous_frame();

    _frame_graph.bind_attachment(*_swapchain_buffer, frame_texture);
    uint64_t const fence_value = _frame_graph.execute(_context.device());
    _context.graphics_fence_value(fence_value);

    _context.swap_buffers();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

    
}

/*
    _pipeline_test = _backend->create_pipeline(
        _pbr_layout,
        vertex_declaration,
        _shader_cache.shader_effect("basic"_hash).shaders(),
        RasterizerDesc { FillMode::Solid, CullMode::Back },
        DepthStencilDesc { CompareOp::Always, false },
        BlendDesc { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
        context.render_pass(),
        "Pipeline_TEST"_hash
    );
*/

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count) {

    _gbuffer_color_buffer = &_frame_graph.add_attachment(
        AttachmentDesc {
            .name = "gbuffer_color",
            .format = backend::Format::RGBA8,
            .width = width,
            .height = height,
            .flags = backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource
        }
    );

    CreateColorInfo gbuffer_color_info {
        .attachment = _gbuffer_color_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = Color(0.4f, 0.2f, 0.1f, 1.0f)
    };

    _frame_graph.add_pass(
        RenderPassDesc {
            .name = "gbuffer",
            .width = width,
            .height = height,
            .color_infos = std::span<CreateColorInfo const>(&gbuffer_color_info, 1)
        },
        [&](RenderPassContext const& context) {
            
            
        }
    );

    _swapchain_buffer = &_frame_graph.add_attachment("swapchain_buffer", backend::MemoryState::Present, backend::MemoryState::Present);

    CreateColorInfo swapchain_info = {
        .attachment = _swapchain_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = Color(0.4f, 0.2f, 0.1f, 1.0f)
    };

    _frame_graph.add_pass(
        RenderPassDesc {
            .name = "present_only_pass",
            .width = width,
            .height = height,
            .color_infos = std::span<CreateColorInfo const>(&swapchain_info, 1)
        },
        RenderPassDefaultFunc
    );

    _frame_graph.build(_context.device(), buffered_frame_count);
}