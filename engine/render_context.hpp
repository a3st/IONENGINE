// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "linked_device.hpp"
#include "math/matrix.hpp"

namespace ionengine
{
    class RenderContext
    {
      public:
        RenderContext(LinkedDevice& device) : device(&device)
        {
            std::vector<rhi::RenderPassColorInfo> colors = {
                rhi::RenderPassColorInfo{.texture = device.get_back_buffer(),
                                         .load_op = rhi::RenderPassLoadOp::Clear,
                                         .store_op = rhi::RenderPassStoreOp::Store,
                                         .clear_color = math::Color(0.2f, 0.3f, 0.5f, 1.0f)}};

            device.get_graphics_context().barrier(device.get_back_buffer(), rhi::ResourceState::Common,
                                                  rhi::ResourceState::RenderTarget);
            device.get_graphics_context().begin_render_pass(colors, std::nullopt);
        }

        ~RenderContext()
        {
            device->get_graphics_context().end_render_pass();
            device->get_graphics_context().barrier(device->get_back_buffer(), rhi::ResourceState::RenderTarget,
                                                   rhi::ResourceState::Common);
        }

        auto draw_mesh(Mesh const& mesh, math::Matrixf const& view_projection) -> void
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
                device->get_graphics_context().bind_vertex_buffer(primitive.vertices, 0,
                                                                  primitive.vertices->get_size());
                device->get_graphics_context().bind_index_buffer(primitive.indices, 0, primitive.indices->get_size(),
                                                                 rhi::IndexFormat::Uint32);
                device->get_graphics_context().draw_indexed(primitive.index_count, 1);
            }

            device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::AllShaderRead,
                                                   rhi::ResourceState::Common);
        }

      private:
        LinkedDevice* device;
    };
} // namespace ionengine