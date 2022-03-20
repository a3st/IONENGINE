// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/math/vector.h>
#include <lib/algorithm.h>

#include <lib/hash/crc32.h>

#include <asset_compiler/obj_loader.h>
#include <asset_compiler/asset_compiler.h>

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

    for(auto& [key, value] : shader_package_data.data) {

        std::cout << std::format("{}", (char*)key.c_str()) << std::endl;
    }

    auto result_desc = ShaderResultDesc {};
    ShaderGraph shader_graph;

    // Basic Shader
    {
        ShaderTemplate shader_template;

        result_desc.domain = ShaderDomain::Surface;
        result_desc.blend_mode = ShaderBlendMode::Opaque;
        result_desc.shader_high = "shader_low_high"_hash;
        result_desc.shader_low = "shader_low_high"_hash;
        result_desc.cull_mode = CullMode::Back;
        result_desc.fill_mode = FillMode::Solid;

        shader_graph
            .input("color_input"_hash, ShaderInput<Vector3f> { u8"color", Vector3f(0.2f, 0.1f, 0.3f) })
            .input("power_input"_hash, ShaderInput<float> { u8"power", 1.0f, { -1.0f, 2.0f } })
            .shader("basic_vertex"_hash, shader_package_data.data.at(u8"basic_vertex").data, ShaderFlags::Vertex)
            .shader("basic_pixel"_hash, shader_package_data.data.at(u8"basic_pixel").data, ShaderFlags::Pixel)
            .shader_pass(
                "shader_low_high"_hash, 
                0, 
                ShaderPassDesc{}
                    .const_buffer("color_input"_hash, 0, 0)
                    .const_buffer("power_input"_hash, 0, sizeof(Vector3f))
            )
            .build(*_backend, result_desc, shader_template);

        // shader_templates["basic"_hash] = std::move(shader_template);
    }
    shader_graph.reset();

    // ...

    // std::unordered_map<uint32_t, ShaderTemplate> shader_templates;
    // std::unordered_map<

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
                .name(u8"Main Render Pass")
                .rect(800, 600)
                .color("swapchain"_hash, RenderPassLoadOp::Clear, Color(0.9f, 0.5f, 0.3f, 1.0f))
                .color("blit"_hash, RenderPassLoadOp::Clear, Color(0.2f, 0.4f, 0.3f, 1.0f)),
            [&](Handle<RenderPass> const& render_pass, RenderPassResources const& resources) {
                
                /*

                Handle<Pipeline> pipeline = get_pipeline_data(
                    shader_template, 
                    render_pass, 
                    index_pass
                );

                encoder.bind_pipeline(pipeline);
                
                */
            }
        )
        .build(*_backend, 2);
}
