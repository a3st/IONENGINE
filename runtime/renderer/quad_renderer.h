// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer.h"

#include "lib/memory.h"

#include "pass_cache.h"

namespace ionengine::renderer {

using namespace memory_literals;

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer(api::Device& device, api::Swapchain& swapchain, const uint32 buffer_count) : m_device(device), m_swapchain(swapchain), 
        m_buffer_count(buffer_count), m_frame_index(0) {

        m_frame_graph = std::make_unique<fg::FrameGraph>(device);

        std::vector<api::DescriptorPoolSize> pool_sizes = { 
            { api::ViewType::ConstantBuffer, 10 },
            { api::ViewType::RenderTarget, 10 }
        };

        m_descriptor_pools.emplace_back(m_device.get().create_descriptor_pool(pool_sizes));

        for(uint32 i = 0; i < buffer_count; ++i) {
            api::ViewDesc view_desc = { api::ViewType::RenderTarget, api::ViewDimension::Texture2D };
            m_swapchain_views.emplace_back(m_device.get().create_view(*m_descriptor_pools.back(), m_swapchain.get().get_back_buffer(i), view_desc));
        }

        api::ResourceDesc cbv_res_desc{};
        cbv_res_desc.dimension = api::ViewDimension::Buffer;
        cbv_res_desc.width = 32768;
        cbv_res_desc.array_size = 1;
        cbv_res_desc.height = 1;
        cbv_res_desc.mip_levels = 1;
        cbv_res_desc.sample_count = 1;
        cbv_res_desc.flags = api::ResourceFlags::ConstantBuffer;
        m_cbv_buffer = m_device.get().create_resource(api::ResourceType::Buffer, cbv_res_desc);

        m_test_memory = m_device.get().allocate_memory(api::MemoryType::Upload, 24_mb, 0, api::ResourceFlags::VertexBuffer);

        m_cbv_buffer->bind_memory(*m_test_memory, 0);

        std::vector<uint32> numbers = { 2, 3, 1, 4, 5 };
        byte* dst = m_cbv_buffer->map();
        std::memcpy(dst, numbers.data(), numbers.size() * sizeof(uint32));
        m_cbv_buffer->unmap();

        for(uint32 i = 0; i < buffer_count; ++i) {
            m_command_lists.emplace_back(m_device.get().create_command_list(api::CommandListType::Graphics));
        }

        m_frame_graph->create_resource("depthbuffer", fg::ResourceType::Attachment, *m_swapchain_views[0], fg::ResourceFlags::DepthStencil);
        m_frame_graph->create_resource("swapchain", fg::ResourceType::Attachment, *m_swapchain_views[0], fg::ResourceFlags::Present);
        m_frame_graph->create_resource("gbuffer", fg::ResourceType::Attachment, *m_swapchain_views[0], fg::ResourceFlags::None);

        /*struct DepthPassData {
            fg::ResourceHandle output;
        };

        m_frame_graph->add_task<DepthPassData>(
            "DepthPass",
            [&](fg::RenderPassBuilder& builder, DepthPassData& data) {
                data.output = builder.write("depthbuffer", fg::ResourceOp::Clear, { 150, 105, 150, 255 } );
            },
            [=](fg::RenderPassContext& context, const DepthPassData& data) {
            }
        );

        struct BasicPassData {
            fg::ResourceHandle input;
            fg::ResourceHandle output;
        };

        m_frame_graph->add_task<BasicPassData>(
            "BasicPass",
            [&](fg::RenderPassBuilder& builder, BasicPassData& data) {
                data.input = builder.read("depthbuffer");
                data.output = builder.write("swapchain", fg::ResourceOp::Clear, { 150, 105, 150, 255 } );
            },
            [=](fg::RenderPassContext& context, const BasicPassData& data) {
            }
        );*/

        struct BasicPassData {
            fg::ResourceHandle output;
        };

        m_frame_graph->add_task<BasicPassData>(
            "BasicPass",
            [&](fg::RenderPassBuilder& builder, BasicPassData& data) {
                data.output = builder.write("swapchain", fg::ResourceOp::Clear, { 0.5f, 0.6f, 0.4f, 1.0f } );
            },
            [=](fg::RenderPassContext& context, const BasicPassData& data) {
            }
        );

        m_frame_graph->compile();
    }

    void tick() override {

        m_frame_graph->update_resource("swapchain", *m_swapchain_views[m_swapchain.get().get_back_buffer_index()]);

        m_frame_graph->execute(*m_command_lists[m_frame_index]);

        m_frame_index = (m_frame_index + 1) % m_buffer_count;
    }

private:

    std::reference_wrapper<api::Device> m_device;
    std::reference_wrapper<api::Swapchain> m_swapchain;

    std::unique_ptr<fg::FrameGraph> m_frame_graph;

    std::vector<std::unique_ptr<api::DescriptorPool>> m_descriptor_pools;
    std::vector<std::unique_ptr<api::View>> m_swapchain_views;

    std::unique_ptr<api::Resource> m_cbv_buffer;
    std::unique_ptr<api::Memory> m_test_memory;
    std::unique_ptr<api::Pipeline> m_test_pipeline;

    std::vector<std::unique_ptr<api::CommandList>> m_command_lists;

    uint32 m_frame_index;
    uint32 m_buffer_count;
};

}