// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>

#include <lib/algorithm.h>
#include <lib/hash/crc32.h>

using namespace ionengine;
using namespace ionengine::renderer;

Renderer::Renderer(Backend& backend, ThreadPool& thread_pool, ShaderPackageData const& shader_package_data) 
    : _backend(&backend), _thread_pool(&thread_pool) {

    // INITIALIZE PARAMETERS WITHOUT CONFIG FILE/OR CONSOLE VARIABLES
    _frame_count = 2;
    _sample_count = 1;
    

    initialize_layouts();
    initialize_shaders(shader_package_data);
    initialize_resources_per_frame();
    build_frame_graph(768, 522, _sample_count, _frame_count);

    // TEST
    test_pipelines.resize(2);

    std::vector<float> vertices = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    std::vector<uint32_t> indices = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

    test_vertex_triangle = backend.create_buffer(65536, BufferFlags::VertexBuffer | BufferFlags::HostWrite);
    test_index_triangle = backend.create_buffer(65536, BufferFlags::IndexBuffer | BufferFlags::HostWrite);
    //test_descriptor_set = backend.create_descriptor_set(_layouts[LayoutType::GBuffer]);

    backend.upload_buffer_data(test_vertex_triangle, 0, std::span<char8_t const>(reinterpret_cast<char8_t const*>(vertices.data()), vertices.size() * sizeof(float)));
    backend.upload_buffer_data(test_index_triangle, 0, std::span<char8_t const>(reinterpret_cast<char8_t const*>(indices.data()), indices.size() * sizeof(uint32_t)));
}

void Renderer::update() {

    Handle<Texture> swapchain_texture = _backend->acquire_next_texture();

    _backend->wait(_graphics_fence_results[_frame_index]);

    _frame_graph
        .bind_external_attachment("swapchain"_hash, swapchain_texture);

    _graphics_fence_results[_frame_index] = _frame_graph.execute(*_backend, _graphics_encoders[_frame_index]);

    _frame_index = (_frame_index + 1) % _frame_count;
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

    _frame_graph.reset(*_backend);
    _backend->recreate_swapchain(width, height, {});
    build_frame_graph(width, height, 1, 2);
}

void Renderer::initialize_resources_per_frame() {

    for(uint32_t i = 0; i < _frame_count; ++i) {

        _graphics_encoders.emplace_back(*_backend, EncoderFlags::Graphics);
        _copy_encoders.emplace_back(*_backend, EncoderFlags::Copy);
    }

    _graphics_fence_results.resize(_frame_count);
    _copy_fence_results.resize(_frame_count);
}

void Renderer::initialize_shaders(ShaderPackageData const& shader_package_data) {

    _shader_cache.create_shader_effect(
        *_backend, 
        "basic"_hash, 
        ShaderEffectDesc{}
            .set_shader_code(u8"basic_vertex", shader_package_data.data.at(u8"basic_vertex"), ShaderFlags::Vertex)
            .set_shader_code(u8"basic_pixel", shader_package_data.data.at(u8"basic_pixel"), ShaderFlags::Pixel)
            .set_binding("view"_hash, ShaderBinding<Buffer> { 0, u8"View CBuffer" })
            .set_binding("material"_hash, ShaderBinding<Buffer> { 1, u8"Material CBuffer" })
    );

    _shader_cache.create_shader_effect(
        *_backend, 
        "gbuffer"_hash, 
        ShaderEffectDesc{}
            .set_shader_code(u8"gbuffer_vertex", shader_package_data.data.at(u8"gbuffer_vertex"), ShaderFlags::Vertex)
            .set_shader_code(u8"gbuffer_pixel", shader_package_data.data.at(u8"gbuffer_pixel"), ShaderFlags::Pixel)
            .set_binding("view"_hash, ShaderBinding<Buffer> { 0, u8"View CBuffer" })
    );



    /*
    ShaderEffectBinder binder(shader_effect);
    binder
        .bind("world"_hash, INVALID_HANDLE(Buffer))
        .bind("material"_hash, INVALID_HANDLE(Buffer));
        //.update(*_backend, INVALID_HANDLE(DescriptorSet));
    */
}  

void Renderer::initialize_layouts() {

    auto ranges = std::vector<DescriptorRangeDesc> {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 1, ShaderFlags::Vertex },
        DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 1, 1, ShaderFlags::Vertex }
    };

    _layouts[LayoutType::ZPass] = _backend->create_descriptor_layout(ranges);

    ranges = std::vector<DescriptorRangeDesc> {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 1, ShaderFlags::Vertex }
    };

    _layouts[LayoutType::GBuffer] = _backend->create_descriptor_layout(ranges);
}

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint16_t const sample_count, uint32_t const buffer_count) {

    _frame_graph
        /*

            Z Render Pass

        */
        .attachment("z_buffer"_hash, Format::D32, width, height, TextureFlags::DepthStencil)
        .render_pass(
            "z_pass"_hash, 
            RenderPassDesc{}
                .set_name(u8"Z Render Pass")
                .set_rect(width, height)
                .set_depth_stencil("z_buffer"_hash, RenderPassLoadOp::Clear, 0.0f, 0x0),
            [&, this](RenderPassContext const& context) {

                
            }
        )
        /*

            G Buffer Pass

        */
        .attachment("albedo"_hash, Format::RGBA8, width, height, TextureFlags::RenderTarget | TextureFlags::ShaderResource)
        .attachment("normal"_hash, Format::RGBA8, width, height, TextureFlags::RenderTarget | TextureFlags::ShaderResource)
        .attachment("metalic"_hash, Format::RGBA8, width, height, TextureFlags::RenderTarget | TextureFlags::ShaderResource)
        .attachment("roughness"_hash, Format::RGBA8, width, height, TextureFlags::RenderTarget | TextureFlags::ShaderResource)
        .attachment("ao"_hash, Format::RGBA8, width, height, TextureFlags::RenderTarget | TextureFlags::ShaderResource)
        .render_pass(
            "gbuffer_pass"_hash, 
            RenderPassDesc{}
                .set_name(u8"GBuffer Render Pass")
                .set_rect(width, height)
                .set_color("albedo"_hash, RenderPassLoadOp::Clear, Color(0.0f, 0.0f, 0.0f, 1.0f))
                .set_color("normal"_hash, RenderPassLoadOp::Clear, Color(0.5f, 0.5f, 1.0f, 1.0f))
                .set_color("metalic"_hash, RenderPassLoadOp::Clear, Color(0.2f, 0.1f, 0.2f, 1.0f))
                .set_color("roughness"_hash, RenderPassLoadOp::Clear, Color(1.0f, 1.0f, 1.0f, 1.0f))
                .set_color("ao"_hash, RenderPassLoadOp::Clear, Color(1.0f, 1.0f, 1.0f, 1.0f))
                .set_depth_stencil("z_buffer"_hash, RenderPassLoadOp::Load),
            [&, this](RenderPassContext const& context) {

                if(test_pipelines[context.frame_index()] == INVALID_HANDLE(Pipeline)) {

                    std::vector<VertexInputDesc> vertex_declaration = {
                        VertexInputDesc { "POSITION", 0, Format::RGB32, 0, sizeof(float) * 3 },
                        VertexInputDesc { "TEXCOORD", 1, Format::RG32, 1, sizeof(float) * 2 },
                        VertexInputDesc { "NORMAL", 2, Format::RGB32, 1, sizeof(float) * 3 }
                    };

                    test_pipelines[context.frame_index()] = _backend->create_pipeline(
                        _layouts[LayoutType::GBuffer],
                        vertex_declaration,
                        _shader_cache.shader_effect("basic"_hash).shaders(),
                        RasterizerDesc { FillMode::Solid, CullMode::Back },
                        DepthStencilDesc { CompareOp::Always, false },
                        BlendDesc { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
                        context.render_pass(),
                        "Pipeline_TEST"_hash
                    );
                }

                context.encoder()
                    .bind_pipeline(test_pipelines[context.frame_index()])
                    //.bind_descriptor_set(test_descriptor_set)
                    .bind_vertex_buffer(0, test_vertex_triangle, 0)
                    .bind_index_buffer(test_index_triangle, 0)
                    .draw_indexed(9, 1, 0);
            }
        )
        /*

            Present Pass

        */
        .external_attachment("swapchain"_hash, Format::RGBA8, MemoryState::Present, MemoryState::Present)
        .render_pass(
            "present_pass"_hash,
            RenderPassDesc{}
                .set_name(u8"Present Render Pass")
                .set_rect(width, height)
                .set_color("swapchain"_hash, RenderPassLoadOp::Clear, Color(0.2f, 0.1f, 0.0f, 1.0f)),
            [&, this](RenderPassContext const& context) {


            }
        )
        .build(*_backend, 2);
}

/*if(_pipeline_test == INVALID_HANDLE(Pipeline)) {

                    std::vector<VertexInputDesc> vertex_declaration = {
                        VertexInputDesc { "POSITION", 0, Format::RGB32, 0, sizeof(float) * 3 },
                        VertexInputDesc { "TEXCOORD", 1, Format::RG32, 1, sizeof(float) * 2 },
                        VertexInputDesc { "NORMAL", 2, Format::RGB32, 1, sizeof(float) * 3 }
                    };

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
                }*/