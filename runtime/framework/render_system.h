// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/wrapper/wrapper.h"
#include "renderer/fg/frame_graph.h"
#include "renderer/quad_renderer.h"

#include "lib/memory.h"

namespace ionengine {

using namespace memory_literals;

class RenderSystem {
public:

    RenderSystem(platform::wnd::Window& window) : m_window(window), m_buffer_count(2) {

        std::cout << format<char>("RenderSystem ({} API) initialized", renderer::api::get_api_name()) << std::endl;
        m_instance = renderer::api::create_unique_instance();

        auto adapters = m_instance->enumerate_adapters();
        m_adapter = std::move(adapters[0]);
        std::cout << format<char>("Selecting default 0 adapter") << std::endl;
        
        std::cout <<
                format<char>(
                    "Vendor Id: {}\nDevice Id: {}\nDevice Name: {}\nDedicated Memory: {}",
                    m_adapter->get_vendor_id(), 
                    m_adapter->get_device_id(), 
                    m_adapter->get_name(), 
                    m_adapter->get_memory()
                ) <<
        std::endl;

        m_device = m_adapter->create_device();

        auto client_size = m_window.get().get_client_size();
        m_swapchain = m_device->create_swapchain(m_window.get().get_handle(), client_size.width, client_size.height, m_buffer_count);

        m_fence_values.resize(m_buffer_count);
        m_fence = m_device->create_fence(0);

        // Renderer class
        m_renderer = std::make_unique<renderer::QuadRenderer>(*m_device, *m_swapchain, m_buffer_count);
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {
        
        uint32 frame_index = m_swapchain->next_buffer(*m_fence, ++m_fence_values[0]);
        m_device->get_command_queue(renderer::api::CommandListType::Graphics).wait(*m_fence, m_fence_values[0]);
        m_fence->wait(m_fence_values[0]);

        m_renderer.get()->tick();

        m_fence->signal(m_fence_values[0]);
        
        m_swapchain->present(*m_fence, m_fence_values[0]);
    }

private:

    std::reference_wrapper<platform::wnd::Window> m_window;

    std::unique_ptr<renderer::api::Instance> m_instance;
    std::unique_ptr<renderer::api::Adapter> m_adapter;
    std::unique_ptr<renderer::api::Device> m_device;
    std::unique_ptr<renderer::api::Swapchain> m_swapchain;
    std::unique_ptr<renderer::api::Fence> m_fence;

    std::vector<uint64> m_fence_values;

    uint32 m_buffer_count;

    std::unique_ptr<renderer::BaseRenderer> m_renderer;
};

std::unique_ptr<RenderSystem> create_unique_render_system(platform::wnd::Window& window) {
    return std::make_unique<RenderSystem>(window);
}

}