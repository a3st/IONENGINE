// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/algorithm.h>
#include <lib/hash/crc32.h>

using namespace ionengine;
using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend& backend, ThreadPool& thread_pool, ShaderPackageData const& shader_package_data) 
    : _backend(&backend), _thread_pool(&thread_pool) {

    for(uint32_t i = 0; i < 3; ++i) {
        _graphics_encoders.emplace_back(backend, EncoderFlags::Graphics);
    }
    
    initialize_shaders(shader_package_data);
    initialize_descriptor_layouts();
    build_frame_graph(800, 600, 1, 2);

    fence_results.resize(2);
    _render_passes.resize(2);
    _pipelines.resize(2);
}

void WorldRenderer::update() {

    Handle<Texture> swapchain_texture = _backend->acquire_next_texture();

    _backend->wait(fence_results[_frame_index]);

    _frame_graph
        .bind_external_attachment("swapchain"_hash, swapchain_texture);

    fence_results[_frame_index] = _frame_graph.execute(*_backend, _graphics_encoders[_frame_index]);

    _frame_index = (_frame_index + 1) % 2;
}

void WorldRenderer::resize(uint32_t const width, uint32_t const height) {

    _frame_graph.reset(*_backend);
    _backend->recreate_swapchain(width, height, {});
    build_frame_graph(width, height, 1, 2);
}

void WorldRenderer::draw_mesh(uint32_t const sort_index, MeshSurfaceData const* const mesh_data, Matrixf const& model) {

    
}

void WorldRenderer::set_projection_view(Matrixf const& projection, Matrixf const& view) {

    
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

void WorldRenderer::initialize_shaders(ShaderPackageData const& shader_package_data) {

    _shader_cache.create_shader_effect(
        *_backend, 
        "basic"_hash, 
        ShaderEffectDesc{}
            .set_shader_code(u8"basic_vertex", shader_package_data.data.at(u8"basic_vertex"), ShaderFlags::Vertex)
            .set_shader_code(u8"basic_pixel", shader_package_data.data.at(u8"basic_pixel"), ShaderFlags::Pixel)
            .set_binding("world"_hash, 0, ShaderBinding<Buffer> { u8"World CBuffer" })
            .set_binding("material"_hash, 1, ShaderBinding<Buffer> { u8"Material CBuffer" })
            .set_binding("wrap_sampler"_hash, 2, ShaderBinding<Sampler> { u8"Sampler Wrap" })
    );

    auto shader_effect = _shader_cache.get_shader_effect("basic"_hash);

    ShaderEffectBinder binder(shader_effect);
    binder
        .bind("world"_hash, INVALID_HANDLE(Buffer))
        .bind("material"_hash, INVALID_HANDLE(Buffer))
        .bind("wrap_sampler"_hash, INVALID_HANDLE(Sampler))
        .update(*_backend, INVALID_HANDLE(DescriptorSet));
}

void WorldRenderer::initialize_descriptor_layouts() {

    auto ranges = std::vector<DescriptorRangeDesc> {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 2, ShaderFlags::Vertex },
        //DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 2, 2, ShaderFlags::Vertex },
        //DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 4, 1, ShaderFlags::Pixel },
        //DescriptorRangeDesc { DescriptorRangeType::Sampler, 5, 1, ShaderFlags::Pixel }
    };
    _pbr_layout = _backend->create_descriptor_layout(ranges);
}


void WorldRenderer::build_frame_graph(uint32_t const width, uint32_t const height, uint16_t const sample_count, uint32_t const buffer_count) {

    _frame_graph
        .attachment("blit"_hash, Format::RGBA8, 768, 522, TextureFlags::RenderTarget)
        .external_attachment("swapchain"_hash, Format::RGBA8, MemoryState::Present, MemoryState::Present)
        .render_pass(
            "main_pass"_hash, 
            RenderPassDesc{}
                .set_name(u8"Main Render Pass")
                .set_rect(800, 600)
                .set_color("swapchain"_hash, RenderPassLoadOp::Clear, Color(0.9f, 0.5f, 0.3f, 1.0f))
                .set_color("blit"_hash, RenderPassLoadOp::Clear, Color(0.2f, 0.4f, 0.3f, 1.0f)),
            [&](RenderPassContext const& context) {

                
            }
        )
        .build(*_backend, 2);
}
