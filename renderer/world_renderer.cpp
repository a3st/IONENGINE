// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/math/vector.h>
#include <lib/algorithm.h>

using namespace ionengine;
using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend& backend, platform::Window& window, ThreadPool& thread_pool) 
    : _backend(&backend), _thread_pool(&thread_pool) {

    Device graphics_device(backend, EncoderType::Graphics, SwapchainDesc { &window, 1, 2 });
    _devices[EncoderType::Graphics] = std::move(graphics_device);

    Device copy_device(backend, EncoderType::Copy);
    _devices[EncoderType::Copy] = std::move(copy_device);

    _render_passes.resize(2);
    _pipelines.resize(2);

    auto ranges = std::vector<DescriptorRangeDesc> {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 2, ShaderFlags::Vertex },
        DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 2, 2, ShaderFlags::Vertex },
        DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 4, 1, ShaderFlags::Pixel },
        DescriptorRangeDesc { DescriptorRangeType::Sampler, 5, 1, ShaderFlags::Pixel }
    };
    _pbr_layout = _backend->create_descriptor_layout(ranges);

    size_t shader_vert_size = get_file_size("shaders/basic_vert.bin");

    std::vector<char8_t> shader_vert;
    shader_vert.resize(shader_vert_size);

    size_t shader_frag_size = get_file_size("shaders/basic_frag.bin");

    std::vector<char8_t> shader_frag;
    shader_frag.resize(shader_frag_size);

    load_bytes_from_file("shaders/basic_vert.bin", shader_vert, std::ios::binary);
    load_bytes_from_file("shaders/basic_frag.bin", shader_frag, std::ios::binary);

    auto shader_datas = std::vector<ShaderData> {
        { u8"basic_vert", ShaderFlags::Vertex, shader_vert },
        { u8"basic_frag", ShaderFlags::Pixel, shader_frag }
    };

    auto result_desc = ShaderResultDesc {};
    result_desc.domain = ShaderDomain::Surface;
    result_desc.blend_mode = ShaderBlendMode::Opaque;
    result_desc.shader_high = 0;
    result_desc.shader_low = 0;
    result_desc.cull_mode = CullMode::Back;
    result_desc.fill_mode = FillMode::Solid;

    ShaderTemplate shader_template;

    ShaderGraph shader_graph;
    shader_graph
        .input(0 /* NodeInputId */, ShaderInput<Vector3f> { u8"color", Vector3f(0.2f, 0.1f, 0.3f) })
        .input(1 /* NodeInputId */, ShaderInput<float> { u8"power", 1.0f, { -1.0f, 2.0f } })
        .shader(
            0, // NodeOutputId
            0, // Pass index
            ShaderDesc{}
                .name(u8"basic") // Shader name
                .shaders(shader_datas) // Shader data
                .input(0 /* NodeInputId */, 0 /* Index */)
        )
        .build(*_backend, result_desc, shader_template);

    _frame_graph
        .attachment(std::to_underlying(AttachmentIds::Blit), Format::RGBA8, 768, 522)
        .external_attachment(std::to_underlying(AttachmentIds::Swapchain), Format::RGBA8, MemoryState::Present, MemoryState::Present)
        .render_pass(
            std::to_underlying(RenderPassIds::Main), 
            RenderPassDesc{}
                .name(u8"MainPass")
                .rect(800, 600)
                .color(std::to_underlying(AttachmentIds::Swapchain), RenderPassLoadOp::Clear, Color(0.9f, 0.5f, 0.3f, 1.0f))
                .color(std::to_underlying(AttachmentIds::Blit), RenderPassLoadOp::Clear, Color(0.2f, 0.4f, 0.3f, 1.0f)),
            [&](Handle<RenderPass> const& render_pass, RenderPassResources const& resources) {
                

            }
        )
        .build(*_backend, 2);

    /*Encoder encoder_graphics(*_backend, EncoderType::Graphics);
    encoder_graphics
        .set_viewport(0, 0, 800, 600)
        .set_scissor(0, 0, 800, 600)
        // ...
    ;

    Device device_graphics(*_backend, EncoderType::Graphics);
    FenceResultInfo result = device_graphics.submit(std::span<Encoder const>(&encoder_graphics, 1));
    device_graphics.wait(result);

    if(device_graphics.is_completed(result)) {
        encoder_graphics
            .set_viewport(0, 0, 800, 600)
            .set_scissor(0, 0, 800, 600)
            // ...
        ;
    }

    _backend->swap_buffers();*/
}

void WorldRenderer::update() {

    //_frame_graph
    //    .bind_external_attachment(std::to_underlying(AttachmentIds::Swapchain), backend.swap_buffer())
    //    .execute(backend);

    frame_index = (frame_index + 1) % 2;
}

void WorldRenderer::resize(uint32_t const width, uint32_t const height) {

    _frame_graph.reset(*_backend);

    //_backend->resize_buffers(width, height, 2);

    _frame_graph
        .build(*_backend, 2);
}

void WorldRenderer::draw_mesh(uint32_t const sort_index, MeshData const* const mesh_data, Matrixf const& model) {

    _meshes[sort_index] = mesh_data;
}

void WorldRenderer::set_projection_view(Matrixf const& projection, Matrixf const& view) {

    _prev_world_buffer = _world_buffer;

    _world_buffer.projection = Matrixf(projection).transpose();
    _world_buffer.view = Matrixf(view).transpose();
}


/*

                    _backend->create_pipeline(
                        _pbr_layout,
                        MeshData::vertex_declaration,
                        _shaders,
                        RasterizerDesc { FillMode::Solid, CullMode::Back },
                        DepthStencilDesc { CompareOp::Always, false },
                        BlendDesc { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
                        render_pass
                    );

*/