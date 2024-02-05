// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer/renderer.hpp"
#include "core/exception.hpp"
#include "math/matrix.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine::renderer
{
    Renderer::Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window)
        : device(rhi::Device::create(rhi::BackendType::DirectX12, window)), render_pipeline(render_pipeline),
          width(window.get_width()), height(window.get_height()),
          resource_allocator(
              device->create_allocator(512 * 1024, 128 * 1024 * 1024, rhi::MemoryAllocatorUsage::Default)),
          render_graph(nullptr), is_graph_initialized(false)
    {
        render_pipeline->initialize(&render_task_stream);

        std::vector<uint8_t> shader_bytes;
        {
            std::ifstream ifs("shaders/basic.bin", std::ios::binary);
            ifs.seekg(0, std::ios::end);
            auto size = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            shader_bytes.resize(size);
            ifs.read(reinterpret_cast<char* const>(shader_bytes.data()), size);
        }

        test_shader = device->create_shader(shader_bytes);

        {
            std::ifstream ifs("shaders/quad.bin", std::ios::binary);
            ifs.seekg(0, std::ios::end);
            auto size = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            shader_bytes.resize(size);
            ifs.read(reinterpret_cast<char* const>(shader_bytes.data()), size);
        }

        quad_shader = device->create_shader(shader_bytes);
    }

    auto Renderer::update(float const dt) -> void
    {
    }

    auto Renderer::render(std::span<core::ref_ptr<Camera>> const targets) -> void
    {
        if (!is_graph_initialized)
        {
            RenderGraphBuilder builder;
            {
                for (auto& target : targets)
                {
                    target->resize(&device, resource_allocator, width, height);
                    if (target->is_render_to_texture())
                    {
                        render_pipeline->setup(builder, target, test_shader);
                    }
                    else
                    {
                        render_pipeline->setup(builder, target, test_shader);

                        builder.add_graphics_pass(
                            "PresentPass",
                            width,
                            height,
                            {
                                "ForwardColor"
                            },
                            {
                                RGAttachmentInfo {
                                    "Swapchain",
                                    RGColorAttachment {
                                        rhi::RenderPassLoadOp::Clear,
                                        rhi::RenderPassStoreOp::Store,
                                        math::Color(0.0f, 0.0f, 0.0f, 1.0f)
                                    }
                                }
                            },
                            [&](RGRenderPassContext& ctx) {
                                ctx.get_command_buffer().set_graphics_pipeline_options(
                                    quad_shader,
                                    rhi::RasterizerStageInfo {
                                        .fill_mode = rhi::FillMode::Solid,
                                        .cull_mode = rhi::CullMode::Back
                                    },
                                    rhi::BlendColorInfo::Opaque(),
                                    std::nullopt
                                );

                                ctx.bind_attachment_as(0, "ForwardTexture");
                                ctx.get_command_buffer().draw(3, 1, 0);
                            }
                        );
                    }
                }
            }
            render_graph = builder.build(&device);
            is_graph_initialized = true;
        }

        render_graph->execute();
        render_task_stream.flush();
    }

    auto Renderer::resize(uint32_t const width, uint32_t const height) -> void
    {
        // device->resize_swapchain_buffers(width, height);
        // this->width = width;
        // this->height = height;
        // is_graph_initialized = false;
    }

    auto Renderer::create_render_target(uint32_t const width, uint32_t const height) -> core::ref_ptr<RenderTarget>
    {
        return core::make_ref<RenderTarget>(&device, resource_allocator, width, height);
    }

    auto Renderer::create_camera(CameraProjectionType const projection_type, uint32_t const resolution_width,
                                 uint32_t const resolution_height) -> core::ref_ptr<Camera>
    {
        return core::make_ref<Camera>(&device, resource_allocator, projection_type, resolution_width,
                                      resolution_height);
    }

    auto Renderer::create_primitive(uint32_t const index_count, std::span<uint8_t const> const vertices,
                                    std::span<uint8_t const> const indices, bool const immediate)
        -> core::ref_ptr<Primitive>
    {
        return core::make_ref<Primitive>(&device, resource_allocator, index_count, vertices, indices, immediate);
    }

    auto Renderer::create_texture(uint32_t const width, uint32_t const height, uint32_t const depth,
                                  uint32_t const mip_levels, rhi::TextureFormat const format,
                                  rhi::TextureDimension const dimension,
                                  std::vector<std::span<uint8_t const>> const& data, bool const immediate)
        -> core::ref_ptr<Texture>
    {
        return core::make_ref<Texture>(&device, resource_allocator, width, height, depth, mip_levels, format, dimension,
                                       data, immediate);
    }
} // namespace ionengine::renderer