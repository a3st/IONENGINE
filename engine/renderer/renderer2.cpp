// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(LinkedDevice& device) : device(&device), rendererWidth(800), rendererHeight(600)
    {
    }

    auto Renderer::createTextureAsset(uint32_t const width, uint32_t const height,
                                      TextureUsage const usage) -> core::ref_ptr<TextureAsset>
    {
        auto createdAsset = core::make_ref<TextureAsset>(*device);
        createdAsset->create(width, height, usage);
        return createdAsset;
    }

    auto Renderer::createShaderAsset() -> core::ref_ptr<ShaderAsset>
    {
        auto createdAsset = core::make_ref<ShaderAsset>(*device);
        return createdAsset;
    }

    auto Renderer::drawQuad() -> void
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
                                       .loadOp = rhi::RenderPassLoadOp::Clear,
                                       .storeOp = rhi::RenderPassStoreOp::Store,
                                       .clearColor = clearColor.value()};
            }
            else
            {
                renderPassColorInfo = {
                    .texture = color, .loadOp = rhi::RenderPassLoadOp::Load, .storeOp = rhi::RenderPassStoreOp::Store};
            }

            renderPassColorInfos.emplace_back(renderPassColorInfo);
        }

        if (depthStencil)
        {
            rhi::RenderPassDepthStencilInfo renderPassDepthStencilInfo = {.texture = depthStencil};
            if (clearDepth.has_value())
            {
                renderPassDepthStencilInfo.clearDepth = clearDepth.value();
                renderPassDepthStencilInfo.depthLoadOp = rhi::RenderPassLoadOp::Clear;
                renderPassDepthStencilInfo.depthStoreOp = rhi::RenderPassStoreOp::Store;
            }
            else
            {
                renderPassDepthStencilInfo.depthLoadOp = rhi::RenderPassLoadOp::Load;
                renderPassDepthStencilInfo.depthStoreOp = rhi::RenderPassStoreOp::Store;
            }

            if (clearStencil.has_value())
            {
                renderPassDepthStencilInfo.clearStencil = clearStencil.value();
                renderPassDepthStencilInfo.stencilLoadOp = rhi::RenderPassLoadOp::Clear;
                renderPassDepthStencilInfo.stencilStoreOp = rhi::RenderPassStoreOp::Store;
            }
            else
            {
                renderPassDepthStencilInfo.stencilLoadOp = rhi::RenderPassLoadOp::Load;
                renderPassDepthStencilInfo.stencilStoreOp = rhi::RenderPassStoreOp::Store;
            }

            device->getGraphicsContext().beginRenderPass(renderPassColorInfos, renderPassDepthStencilInfo);
        }
        else
        {
            device->getGraphicsContext().beginRenderPass(renderPassColorInfos, std::nullopt);
        }

        device->getGraphicsContext().setViewport(0, 0, rendererWidth, rendererHeight);
        device->getGraphicsContext().setScissor(0, 0, rendererWidth, rendererHeight);
    }

    auto Renderer::endDraw() -> void
    {
        device->getGraphicsContext().endRenderPass();

        while (!colorBarriers.empty())
        {
            auto texture = std::move(colorBarriers.top());
            colorBarriers.pop();
            device->getGraphicsContext().barrier(texture, rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);
        }

        currentShader = nullptr;
    }

    auto Renderer::render(std::span<RenderableData> const renderables) -> void
    {
        std::vector<size_t> indices(renderables.size());
        std::iota(indices.begin(), indices.end(), 0);

        // Sort renderables by shader
        std::sort(indices.begin(), indices.end(), [&](auto const lhs, auto const rhs) {
            return renderables[lhs].shader->getHash() < renderables[rhs].shader->getHash();
        });

        // Apply sort to array
        for (auto const i : std::views::iota(0u, renderables.size()))
        {
            renderables[i].shaderIndex = indices[i];
        }

        std::stack<rhi::BufferBindData> resolveBuffers;
        std::stack<rhi::TextureBindData> resolveTextures;

        for (auto const index : indices)
        {
            RenderableData const& renderableData = renderables[index];

            if (currentShader != renderableData.shader)
            {
                device->getGraphicsContext().setGraphicsPipelineOptions(
                    renderableData.shader, renderableData.rasterizerStage, renderableData.blendColor,
                    renderableData.depthStencil);

                currentShader = renderableData.shader;
            }

            for (auto const& [boundSlot, boundData] : renderableData.boundedBuffers)
            {
                if (!(boundData.resource->getFlags() & (rhi::BufferUsageFlags)rhi::BufferUsage::MapWrite))
                {
                    rhi::ResourceState curResourceState;
                    rhi::ResourceState nextResourceState;
                    switch (boundData.usage)
                    {
                        case rhi::BufferUsage::ConstantBuffer: {
                            curResourceState = rhi::ResourceState::Common;
                            nextResourceState = rhi::ResourceState::AllShaderRead;
                            break;
                        }
                        case rhi::BufferUsage::ShaderResource: {
                            curResourceState = rhi::ResourceState::Common;
                            nextResourceState = rhi::ResourceState::AllShaderRead;
                            break;
                        }
                        case rhi::BufferUsage::UnorderedAccess: {
                            curResourceState = rhi::ResourceState::Common;
                            nextResourceState = rhi::ResourceState::UnorderedAccess;
                            break;
                        }
                    }

                    device->getGraphicsContext().barrier(boundData.resource, curResourceState, nextResourceState);
                    resolveBuffers.push(boundData);
                }

                uint64_t const descriptorOffset = boundData.resource->getDescriptorOffset(boundData.usage);
                device->getGraphicsContext().bindDescriptor(boundSlot, descriptorOffset);
            }

            switch (renderableData.renderableType)
            {
                case RenderableType::Quad: {
                    this->drawQuad();
                    break;
                }
                case RenderableType::Surface: {

                    break;
                }
                case RenderableType::Compute: {

                    break;
                }
            }

            while (!resolveBuffers.empty())
            {
                auto boundData = std::move(resolveBuffers.top());
                resolveBuffers.pop();

                if (!(boundData.resource->getFlags() & (rhi::BufferUsageFlags)rhi::BufferUsage::MapWrite))
                {
                    rhi::ResourceState curResourceState;
                    rhi::ResourceState nextResourceState;
                    switch (boundData.usage)
                    {
                        case rhi::BufferUsage::ConstantBuffer: {
                            curResourceState = rhi::ResourceState::AllShaderRead;
                            nextResourceState = rhi::ResourceState::Common;
                            break;
                        }
                        case rhi::BufferUsage::ShaderResource: {
                            curResourceState = rhi::ResourceState::AllShaderRead;
                            nextResourceState = rhi::ResourceState::Common;
                            break;
                        }
                        case rhi::BufferUsage::UnorderedAccess: {
                            curResourceState = rhi::ResourceState::UnorderedAccess;
                            nextResourceState = rhi::ResourceState::Common;
                            break;
                        }
                    }

                    device->getGraphicsContext().barrier(boundData.resource, curResourceState, nextResourceState);
                }
            }
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
            //    primitive.vertices->get_size());
    device->get_graphics_context().bind_index_buffer(primitive.indices,
            //    0, primitive.indices->get_size(),
            //                                                     rhi::IndexFormat::Uint32);
            //    device->get_graphics_context().draw_indexed(primitive.index_count, 1);
        }

        // device->get_graphics_context().barrier(material_buffer, rhi::ResourceState::AllShaderRead,
        //                                        rhi::ResourceState::Common);
    }
    */
} // namespace ionengine