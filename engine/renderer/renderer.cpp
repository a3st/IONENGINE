// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(LinkedDevice& device) : device(&device), rendererWidth(800), rendererHeight(600)
    {
    }

    auto Renderer::registerShader(std::string_view const shaderName, core::ref_ptr<ShaderAsset> shaderAsset) -> void
    {
        registeredShaders[std::string(shaderName)] = shaderAsset;
    }

    auto Renderer::setShader(std::string_view const shaderName) -> void
    {
        auto shaderAsset = registeredShaders[std::string(shaderName)];

        device->getGraphicsContext().set_graphics_pipeline_options(
            shaderAsset->getShader(),
            rhi::RasterizerStageInfo{.fill_mode = rhi::FillMode::Solid, .cull_mode = rhi::CullMode::Back},
            rhi::BlendColorInfo::Opaque(), std::nullopt);
    }

    auto Renderer::drawQuad(math::Matrixf const& viewProjection) -> void
    {
        device->getGraphicsContext().draw(3, 1);
    }

    auto Renderer::resize(uint32_t const width, uint32_t const height) -> void
    {
        rendererWidth = width;
        rendererHeight = height;
    }

    auto Renderer::beginDraw(std::span<core::ref_ptr<rhi::Texture>> colors, core::ref_ptr<rhi::Texture> depthStencil,
                             std::optional<math::Color> const clearColor, std::optional<float> const clearDepth,
                             std::optional<uint8_t> const clearStencil) -> void
    {
        std::vector<rhi::RenderPassColorInfo> renderPassColorInfos;
        for (auto color : colors)
        {
            device->getGraphicsContext().barrier(color, rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
            colorBarriers.emplace(color);

            rhi::RenderPassColorInfo renderPassColorInfo;
            if (clearColor.has_value())
            {
                renderPassColorInfo = {.texture = color,
                                       .load_op = rhi::RenderPassLoadOp::Clear,
                                       .store_op = rhi::RenderPassStoreOp::Store,
                                       .clear_color = clearColor.value()};
            }
            else
            {
                renderPassColorInfo = {.texture = color,
                                       .load_op = rhi::RenderPassLoadOp::Load,
                                       .store_op = rhi::RenderPassStoreOp::Store};
            }

            renderPassColorInfos.emplace_back(renderPassColorInfo);
        }

        if (depthStencil)
        {
            rhi::RenderPassDepthStencilInfo renderPassDepthStencilInfo = {.texture = depthStencil};
            if (clearDepth.has_value())
            {
                renderPassDepthStencilInfo.clear_depth = clearDepth.value();
                renderPassDepthStencilInfo.depth_load_op = rhi::RenderPassLoadOp::Clear;
                renderPassDepthStencilInfo.depth_store_op = rhi::RenderPassStoreOp::Store;
            }
            else
            {
                renderPassDepthStencilInfo.depth_load_op = rhi::RenderPassLoadOp::Load;
                renderPassDepthStencilInfo.depth_store_op = rhi::RenderPassStoreOp::Store;
            }

            if (clearStencil.has_value())
            {
                renderPassDepthStencilInfo.clear_stencil = clearStencil.value();
                renderPassDepthStencilInfo.stencil_load_op = rhi::RenderPassLoadOp::Clear;
                renderPassDepthStencilInfo.stencil_store_op = rhi::RenderPassStoreOp::Store;
            }
            else
            {
                renderPassDepthStencilInfo.stencil_load_op = rhi::RenderPassLoadOp::Load;
                renderPassDepthStencilInfo.stencil_store_op = rhi::RenderPassStoreOp::Store;
            }

            device->getGraphicsContext().begin_render_pass(renderPassColorInfos, renderPassDepthStencilInfo);
        }
        else
        {
            device->getGraphicsContext().begin_render_pass(renderPassColorInfos, std::nullopt);
        }

        device->getGraphicsContext().set_viewport(0, 0, rendererWidth, rendererHeight);
        device->getGraphicsContext().set_scissor(0, 0, rendererWidth, rendererHeight);
    }

    auto Renderer::endDraw() -> void
    {
        device->getGraphicsContext().end_render_pass();

        while (!colorBarriers.empty())
        {
            auto texture = std::move(colorBarriers.top());
            colorBarriers.pop();
            device->getGraphicsContext().barrier(texture, rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);
        }
    }

    /*
    auto Renderer::draw_mesh(core::ref_ptr<Mesh> mesh) -> void
    {
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

        // device->get_graphics_context().bind_descriptor("WorldData",
        // test_buffer->get_descriptor_offset(rhi::BufferUsage::ConstantBuffer));

        auto material_buffer = mesh.material->get_buffer();

        device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::Common,
                                               rhi::ResourceState::AllShaderRead);

        device->get_graphics_context().bind_descriptor("MaterialData",
                                                       material_buffer->get(rhi::BufferUsage::ConstantBuffer));

        // for (auto const& primitive : mesh->primitives)
        {
            //    device->get_graphics_context().bind_vertex_buffer(primitive.vertices, 0,
            //    primitive.vertices->get_size()); device->get_graphics_context().bind_index_buffer(primitive.indices,
            //    0, primitive.indices->get_size(),
            //                                                     rhi::IndexFormat::Uint32);
            //    device->get_graphics_context().draw_indexed(primitive.index_count, 1);
        }

        // device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::AllShaderRead,
        //                                        rhi::ResourceState::Common);
    }
    */
} // namespace ionengine