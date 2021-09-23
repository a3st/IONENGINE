// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/wrapper/wrapper.h"
//#include "renderer/fg/frame_graph.h"
//#include "renderer/quad_renderer.h"

#include "lib/memory.h"

namespace ionengine {

using namespace memory_literals;

class RenderSystem {
public:

    RenderSystem(platform::wnd::Window& window) : m_window(window), m_buffer_count(2) {

        m_device = renderer::wrapper::create_unique_device(0);

        auto client_size = m_window.get().get_client_size();
        //m_swapchain = m_device->create_swapchain(m_window.get().get_handle(), client_size.width, client_size.height, m_buffer_count);

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

    std::reference_wrapper<platform::wnd::Window> m_window;

    std::unique_ptr<renderer::wrapper::Device> m_device;

    std::vector<uint64> m_fence_values;

    uint32 m_buffer_count;
};

std::unique_ptr<RenderSystem> create_unique_render_system(platform::wnd::Window& window) {
    return std::make_unique<RenderSystem>(window);
}

}