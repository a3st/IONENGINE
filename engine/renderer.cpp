// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "material.hpp"
#include "precompiled.h"
#include "texture.hpp"

namespace ionengine
{
    struct WorldData
    {
        math::Matrixf model;
        math::Matrixf view;
        math::Matrixf proj;
    };

    Renderer::Renderer(LinkedDevice& device) : device(&device)
    {
        test_buffer = device.get_device().create_buffer(
            sizeof(WorldData), 0, (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::MapWrite);
    }

    auto Renderer::draw_mesh(core::ref_ptr<Mesh> mesh) -> void
    {
        device->get_graphics_context().set_viewport(0, 0, 752, 286);
        device->get_graphics_context().set_scissor(0, 0, 752, 286);

        device->get_graphics_context().set_graphics_pipeline_options(
            mesh->material->get_shader(),
            rhi::RasterizerStageInfo{.fill_mode = rhi::FillMode::Solid, .cull_mode = rhi::CullMode::Back},
            rhi::BlendColorInfo::Opaque(), std::nullopt);

        uint8_t* buffer = test_buffer->map_memory();

        auto world_buffer = WorldData{.model = math::Matrixf::scale(math::Vector3f(1.5f, 1.5f, 1.5f)),
                                      .view = math::Matrixf::look_at_rh(math::Vector3f(5.0f, 2.0f, 0.0f),
                                                                        math::Vector3f(0.0f, 0.0f, 0.0f),
                                                                        math::Vector3f(0.0f, 1.0f, 0.0f)),
                                      .proj = math::Matrixf::perspective_rh(1.04f, 752 / 286, 0.1f, 100.0f)};

        memcpy(buffer, &world_buffer, sizeof(WorldData));
        test_buffer->unmap_memory();

        device->get_graphics_context().bind_descriptor("WorldData", test_buffer->get_descriptor_offset(rhi::BufferUsage::ConstantBuffer));

        /*auto material_buffer = mesh.material->get_buffer();

        device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::Common,
                                               rhi::ResourceState::AllShaderRead);

        device->get_graphics_context().bind_descriptor("MaterialData",
                                                       material_buffer->get(rhi::BufferUsage::ConstantBuffer));*/

        for (auto const& primitive : mesh->primitives)
        {
            device->get_graphics_context().bind_vertex_buffer(primitive.vertices, 0, primitive.vertices->get_size());
            device->get_graphics_context().bind_index_buffer(primitive.indices, 0, primitive.indices->get_size(),
                                                             rhi::IndexFormat::Uint32);
            device->get_graphics_context().draw_indexed(primitive.index_count, 1);
        }

        // device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::AllShaderRead,
        //                                        rhi::ResourceState::Common);
    }

    auto Renderer::draw_quad(math::Matrixf const& view_proj) -> void
    {
    }

    auto Renderer::begin_draw(std::span<core::ref_ptr<Texture> const> const colors,
                              core::ref_ptr<Texture> depth_stencil, math::Color const& clear_color,
                              float const clear_depth, uint8_t const clear_stencil) -> void
    {
        std::vector<rhi::RenderPassColorInfo> render_pass_colors;

        for (auto const& color : colors)
        {
            render_pass_colors.emplace_back(
                rhi::RenderPassColorInfo{.texture = color->get_texture(),
                                         .load_op = rhi::RenderPassLoadOp::Clear,
                                         .store_op = rhi::RenderPassStoreOp::Store,
                                         .clear_color = clear_color});

            render_pass_color_textures.emplace_back(color->get_texture());
        }

        for (auto const& texture : render_pass_color_textures)
        {
            device->get_graphics_context().barrier(texture, rhi::ResourceState::Common,
                                                   rhi::ResourceState::RenderTarget);
        }
        device->get_graphics_context().begin_render_pass(render_pass_colors, std::nullopt);
    }

    auto Renderer::end_draw() -> void
    {
        device->get_graphics_context().end_render_pass();

        for (auto const& texture : render_pass_color_textures)
        {
            device->get_graphics_context().barrier(texture, rhi::ResourceState::RenderTarget,
                                                   rhi::ResourceState::Common);
        }
        render_pass_color_textures.clear();
    }
} // namespace ionengine