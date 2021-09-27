// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/wrapper/wrapper.h"
//#include "renderer/fg/frame_graph.h"
//#include "renderer/quad_renderer.h"

#include "lib/memory.h"

namespace ionengine::renderer {

using namespace memory_literals;

class Renderer {
public:

    Renderer(framework::AppFramework* app_framework) : m_app_framework(app_framework), m_buffer_count(2) {

        auto client_size = m_app_framework->get_client_size();
        m_device = renderer::wrapper::create_unique_device(0, m_app_framework->get_window(), client_size.x, client_size.y, m_buffer_count, 1);

        std::cout << format<char>("Adapter name - {}, memory - {}", m_device->get_adapter_desc().name, m_device->get_adapter_desc().local_memory) << std::endl;

        m_memory = m_device->allocate_memory(renderer::wrapper::MemoryType::Default, 1024_mb, 0, renderer::wrapper::BufferFlags::VertexBuffer);

        //m_fence_values.resize(m_buffer_count);
        //m_fence = m_device->create_fence(0);

        // Renderer class
        //m_renderer = std::make_unique<renderer::QuadRenderer>(*m_device, *m_swapchain, m_buffer_count);
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {


        
        /*uint32 frame_index = m_swapchain->next_buffer(*m_fence, ++m_fence_values[0]);
        m_device->get_command_queue(renderer::api::CommandListType::Graphics).wait(*m_fence, m_fence_values[0]);
        m_fence->wait(m_fence_values[0]);

        m_renderer.get()->tick();

        m_fence->signal(m_fence_values[0]);
        
        m_swapchain->present(*m_fence, m_fence_values[0]);*/
    }

private:

    framework::AppFramework* m_app_framework;

    std::unique_ptr<renderer::wrapper::Device> m_device;

    std::unique_ptr<renderer::wrapper::Memory> m_memory;

    std::vector<uint64> m_fence_values;

    uint32 m_buffer_count;
};

}