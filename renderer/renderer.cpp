// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>

#include <shader/technique.h>

using namespace ionengine::renderer;

Renderer::Renderer(platform::Window& window) : 
    _context(window, 2),
    _frame_graph(_context) {

    build_frame_graph(window.client_size().width, window.client_size().height, 2);

    _buffer_triangle = _context.device().create_buffer(65536, backend::BufferFlags::VertexBuffer);

    std::vector<float> triangles = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f 
    };

    shader::Technique technique("../../data/shaders/techniques/geometry.json5");

    _context.device().create_shader(technique.data().shaders[0].source, backend::ShaderFlags::Vertex);
}

void Renderer::render(scene::Scene& scene) {

    // -> Scene Data
    /*
        for(auto& object : scene.cache_objects[CacheType::Mesh]) {
            // -> renderable_queue.emplace(object, material, data);
        }
    */

    _context.submit_or_skip_upload_buffers();

    auto frame_texture = _context.get_or_wait_previous_frame();

    _frame_graph.bind_attachment(*_swapchain_buffer, frame_texture);
    _frame_graph.execute();

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
        frontend::AttachmentDesc {
            .name = "gbuffer_color",
            .format = backend::Format::RGBA8,
            .width = width,
            .height = height,
            .flags = backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource
        }
    );

    frontend::CreateColorInfo gbuffer_color_info {
        .attachment = _gbuffer_color_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = Color(0.4f, 0.2f, 0.1f, 1.0f)
    };

    _frame_graph.add_pass(
        frontend::RenderPassDesc {
            .name = "gbuffer",
            .width = width,
            .height = height,
            .color_infos = std::span<frontend::CreateColorInfo const>(&gbuffer_color_info, 1)
        },
        [&](frontend::RenderPassContext const& context) {
            
            
        }
    );

    _swapchain_buffer = &_frame_graph.add_attachment("swapchain_buffer", backend::MemoryState::Present, backend::MemoryState::Present);

    frontend::CreateColorInfo swapchain_info = {
        .attachment = _swapchain_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = Color(0.4f, 0.2f, 0.1f, 1.0f)
    };

    _frame_graph.add_pass(
        frontend::RenderPassDesc {
            .name = "present_only_pass",
            .width = width,
            .height = height,
            .color_infos = std::span<frontend::CreateColorInfo const>(&swapchain_info, 1)
        },
        frontend::RenderPassDefaultFunc
    );

    _frame_graph.build_for(buffered_frame_count);
}

void Renderer::load_shader() {

    
}