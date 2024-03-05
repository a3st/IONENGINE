// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "material.hpp"
#include "model.hpp"
#include "precompiled.h"
#include "texture.hpp"

namespace ionengine
{
    Renderer::Renderer(LinkedDevice& device) : device(&device)
    {
    }

    auto Renderer::draw_mesh(Mesh const& mesh, math::Matrixf const& model, math::Matrixf const& view_proj) -> void
    {
        device->get_graphics_context().set_graphics_pipeline_options(
            mesh.material->get_shader(),
            rhi::RasterizerStageInfo{.fill_mode = rhi::FillMode::Solid, .cull_mode = rhi::CullMode::Back},
            rhi::BlendColorInfo::Opaque(), std::nullopt);

        auto material_buffer = mesh.material->get_buffer();

        device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::Common,
                                               rhi::ResourceState::AllShaderRead);

        device->get_graphics_context().bind_descriptor("MaterialData",
                                                       material_buffer->get(rhi::BufferUsage::ConstantBuffer));

        for (auto const& primitive : mesh.primitives)
        {
            device->get_graphics_context().bind_vertex_buffer(primitive.vertices, 0, primitive.vertices->get_size());
            device->get_graphics_context().bind_index_buffer(primitive.indices, 0, primitive.indices->get_size(),
                                                             rhi::IndexFormat::Uint32);
            device->get_graphics_context().draw_indexed(primitive.index_count, 1);
        }

        device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::AllShaderRead,
                                               rhi::ResourceState::Common);
    }

    auto Renderer::draw_quad(math::Matrixf const& view_proj) -> void
    {
    }

    auto Renderer::begin_draw(std::span<core::ref_ptr<Texture> const> const colors, core::ref_ptr<Texture> depth_stencil,
                              math::Color const& clear_color, float const clear_depth, uint8_t const clear_stencil)
        -> void
    {
        std::vector<rhi::RenderPassColorInfo> render_pass_colors;

        for (auto const& color : colors)
        {
            render_pass_colors.emplace_back(
                rhi::RenderPassColorInfo{.texture = color->get_texture(),
                                         .load_op = rhi::RenderPassLoadOp::Clear,
                                         .store_op = rhi::RenderPassStoreOp::Store,
                                         .clear_color = math::Color(0.2f, 0.3f, 0.5f, 1.0f)});

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