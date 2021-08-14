// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/renderer.h"

namespace ionengine::renderer {

class RenderBackend {
public:

    RenderBackend(platform::Window& window) : m_window(window) {

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
        m_swapchain = m_device->create_swapchain(m_window.get().get_native_handle(), client_size.width, client_size.height, m_swap_buffer_count);
    }

private:

    std::reference_wrapper<platform::Window> m_window;

    std::unique_ptr<renderer::Instance> m_instance;
    std::unique_ptr<renderer::Adapter> m_adapter;
    std::unique_ptr<renderer::Device> m_device;
    std::unique_ptr<renderer::Swapchain> m_swapchain;

    uint32 m_swap_buffer_count = 2;

    uint32 m_frame_index = 0;
};

}