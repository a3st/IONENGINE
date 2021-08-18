// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/renderer.h"

namespace ionengine::renderer {

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer(Device& device, Swapchain& swapchain, const uint32 buffer_count) : m_device(device), m_swapchain(swapchain) {

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
    }

    void tick() override {

        m_frame_graph->bind_attachment("swapchain", *m_swapchain_views[m_swapchain.get().get_back_buffer_index()]);

        struct DepthPassData {
            FrameGraphResource output;
        };

        m_frame_graph->add_pass<DepthPassData>("DepthPass",
            [&](RenderPassBuilder& builder, DepthPassData& data) {
                data.output = builder.add_output("swapchain", RenderPassLoadOp::Clear, { 200, 105, 150, 255 });
            },
            [=](RenderPassResources& resources, const DepthPassData& data, RenderPassContext& context) {
                // context.get_command_list().bind_pipeline(*m_test_pipeline);
                // context.get_command_list().draw();
            }
        );

        m_frame_graph->build();
        m_frame_graph->execute();
    }

private:

    std::reference_wrapper<Device> m_device;
    std::reference_wrapper<Swapchain> m_swapchain;

    std::unique_ptr<FrameGraph> m_frame_graph;

    std::vector<std::unique_ptr<DescriptorPool>> m_descriptor_pools;
    std::vector<std::unique_ptr<View>> m_swapchain_views;

    std::unique_ptr<Pipeline> m_test_pipeline;

};

}