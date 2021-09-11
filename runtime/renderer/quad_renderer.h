// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/renderer.h"

namespace ionengine::renderer {

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer(Device& device, Swapchain& swapchain, const uint32 buffer_count) : m_device(device), m_swapchain(swapchain), 
        m_buffer_count(buffer_count), m_frame_index(0) {

        m_frame_graph = std::make_unique<FrameGraph>(device);

        std::vector<DescriptorPoolSize> pool_sizes = { 
            { ViewType::ConstantBuffer, 10 },
            { ViewType::RenderTarget, 10 }
        };

        m_descriptor_pools.emplace_back(m_device.get().create_descriptor_pool(pool_sizes));

        for(uint32 i = 0; i < buffer_count; ++i) {
            ViewDesc view_desc = { ViewType::RenderTarget, ViewDimension::Texture2D };
            m_swapchain_views.emplace_back(m_device.get().create_view(*m_descriptor_pools.back(), m_swapchain.get().get_back_buffer(i), view_desc));
        }

        for(uint32 i = 0; i < buffer_count; ++i) {
            m_command_lists.emplace_back(m_device.get().create_command_list(CommandListType::Graphics));
        }

        m_frame_graph->create_resource("swapchain", { Format::RGBA8unorm, 800, 600 }, *m_swapchain_views[0]);
        m_frame_graph->create_resource("fx", { Format::RGBA8unorm, 800, 600 }, *m_swapchain_views[0]);

        struct DepthPassData {
            FrameGraphResourceHandle output;
        };

        m_frame_graph->add_pass<DepthPassData>(
            "DepthPass",
            [&](RenderPassBuilder& builder, DepthPassData& data) {
                data.output = builder.write("swapchain", RenderPassLoadOp::Clear, RenderPassStoreOp::Store, { 200, 105, 150, 255 } );
            },
            [=](RenderPassContext& context, const DepthPassData& data) {
            }
        );

        struct BasicPassData {
            FrameGraphResourceHandle input;
            FrameGraphResourceHandle output;
        };

        m_frame_graph->add_pass<BasicPassData>(
            "BasicPass",
            [&](RenderPassBuilder& builder, BasicPassData& data) {
                data.input = builder.read("fx");
                data.output = builder.write("swapchain", RenderPassLoadOp::Load, RenderPassStoreOp::Store);
            },
            [=](RenderPassContext& context, const BasicPassData& data) {
            }
        );

        m_frame_graph->compile();
        m_frame_graph->export_dot("quad_renderer_test");
    }

    void tick() override {

        m_frame_graph->update_resource("swapchain", { Format::RGBA8unorm, 800, 600 }, *m_swapchain_views[m_swapchain.get().get_back_buffer_index()]);

        m_frame_graph->execute(*m_command_lists[m_frame_index]);

        m_frame_index = (m_frame_index + 1) % m_buffer_count;
    }

private:

    std::reference_wrapper<Device> m_device;
    std::reference_wrapper<Swapchain> m_swapchain;

    std::unique_ptr<FrameGraph> m_frame_graph;

    std::vector<std::unique_ptr<DescriptorPool>> m_descriptor_pools;
    std::vector<std::unique_ptr<View>> m_swapchain_views;

    std::unique_ptr<Pipeline> m_test_pipeline;

    std::vector<std::unique_ptr<CommandList>> m_command_lists;

    uint32 m_frame_index;
    uint32 m_buffer_count;
};

}