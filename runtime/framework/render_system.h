// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/base/window.h"

#include "renderer/api.h"
#include "renderer/frame_graph.h"
#include "renderer/quad_renderer.h"

#include "lib/memory.h"

namespace ionengine {

using namespace memory_literals;

class RenderSystem {
public:

    RenderSystem(platform::Window& window) : m_window(window), m_buffer_count(2) {

        std::cout << format<std::string>("RenderSystem ({} API) initialized", renderer::get_api_name()) << std::endl;
        m_instance = renderer::create_unique_instance();

        auto adapters = m_instance->enumerate_adapters();
        m_adapter = std::move(adapters[0]);
        std::cout << format<std::string>("Selecting default 0 adapter") << std::endl;
        
        std::cout <<
                format<std::string>(
                    "Vendor Id: {}\nDevice Id: {}\nDevice Name: {}\nDedicated Memory: {}",
                    m_adapter->get_vendor_id(), 
                    m_adapter->get_device_id(), 
                    m_adapter->get_name(), 
                    m_adapter->get_memory()
                ) <<
        std::endl;

        m_device = m_adapter->create_device();

        auto client_size = m_window.get().get_client_size();
        m_swapchain = m_device->create_swapchain(m_window.get().get_native_handle(), client_size.width, client_size.height, m_buffer_count);

        // Renderer class
        m_renderer = std::make_unique<renderer::QuadRenderer>(*m_device, *m_swapchain, m_buffer_count);
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {
        
        m_renderer.get()->tick();
    }

private:

    std::reference_wrapper<platform::Window> m_window;

    std::unique_ptr<renderer::Instance> m_instance;
    std::unique_ptr<renderer::Adapter> m_adapter;
    std::unique_ptr<renderer::Device> m_device;
    std::unique_ptr<renderer::Swapchain> m_swapchain;

    uint32 m_buffer_count;

    std::unique_ptr<renderer::BaseRenderer> m_renderer;
};

}