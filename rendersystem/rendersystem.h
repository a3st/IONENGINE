// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/engine_system.h"

#include "lib/memory.h"
#include "gfx/gfx.h"

#include "limits.h"

#include "framegraph.h"

namespace ionengine::rendersystem {

using namespace memory_literals;

class RenderSystem : public EngineSystem {
public:

    RenderSystem(platform::wnd::Window* window) {

        window->set_label(lib::format<char>("IONENGINE - {}", gfx::api_name));

        auto client = window->get_client_size();

        m_device = gfx::create_unique_device(0, window->get_handle(), client.width, client.height, 2, 1);

        // Initialize frame resources
        m_present_command_lists.resize(2);
        m_fences.resize(2);
        m_fence_values.resize(2);

        for(uint32 i = 0; i < 2; ++i) {
            m_present_command_lists[i] = m_device->create_command_list(gfx::CommandListType::Graphics);
            m_fences[i] = m_device->create_fence(0);
            m_fence_values[i] = 0;
        }

        gfx::AdapterDesc adapter_desc = m_device->get_adapter_desc();
        std::cout << lib::format<char>("Adapter name: {}, Local memory size: {}, Adapter Id: {}, Vendor Id: {}", 
            adapter_desc.name, adapter_desc.local_memory, adapter_desc.device_id, adapter_desc.vendor_id) << std::endl;
        
        m_framegraph = std::make_unique<FrameGraph>(m_device.get());
    }

    void tick() override {

        struct BasicPassData {
            FrameGraphResource* swapchain;
        };

        auto basic_pass = m_framegraph->add_pass<BasicPassData>(
            "BasicPass",
            [&](FrameGraphPassBuilder* builder, BasicPassData& data) {
                data.swapchain = builder->create(FrameGraphResourceType::Attachment, Texture::Format::RGBA8, 800, 600, FrameGraphResourceFlags::Swapchain);
                data.swapchain = builder->write(data.swapchain, FrameGraphResourceOp::Clear, { 0.9f, 0.4f, 0.3f, 1.0f });
            },
            [=](FrameGraphPassContext* context, const BasicPassData& data) {
                
            }
        );

        const uint32 frame_index = m_device->get_swapchain_buffer_index();

        m_framegraph->execute(m_present_command_lists[frame_index].get());

        m_device->present();

        // wait for previous frame
        {
            const uint64 prev_fence_value = m_fence_values[frame_index];
            m_device->signal(gfx::CommandListType::Graphics, m_fences[frame_index].get(), m_fence_values[frame_index]);
            m_fence_values[frame_index]++;

            m_fences[frame_index]->wait(prev_fence_value);
        }
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

private:

    std::unique_ptr<gfx::Device> m_device;

    std::unique_ptr<FrameGraph> m_framegraph;

    std::vector<std::unique_ptr<gfx::CommandList>> m_present_command_lists;
    std::vector<std::unique_ptr<gfx::Fence>> m_fences;
    std::vector<uint64> m_fence_values;
};

}