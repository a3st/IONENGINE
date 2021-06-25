// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/base_renderer.h"
#include "renderer/api.h"
#include "platform/window.h"

namespace ionengine {

class RenderSystem final {
public:

    RenderSystem(const platform::Window& window) : 
        m_adapter(m_instance), 
        m_device(m_adapter), 
        m_window(window),
        m_swapchain(m_instance, m_device, window.get_handle(), 2)
        /*m_descriptors {
            { m_device, 16, false },
            { m_device, 1, false },
            { m_device, 1, false }
        }*/ {

        std::cout << 
            "device_id: " << m_adapter.get_id() <<
            "\ndevice_name: " << m_adapter.get_name() << 
            "\nmemory_device: " << m_adapter.get_memory() <<
        std::endl;
    }

    void resize(const uint32 width, const uint32 height) {
        m_swapchain.resize(width, height);
    }

    void tick() {
        
    }

private:

    renderer::Instance m_instance;
    renderer::Adapter m_adapter;
    renderer::Device m_device;
    renderer::Swapchain m_swapchain;
/*
    struct {
        renderer::DescriptorPool<renderer::DesctiptorType::Sampler> sampler;
        renderer::DescriptorPool<renderer::DesctiptorType::RenderTarget> rt;
        renderer::DescriptorPool<renderer::DesctiptorType::Buffer> buffer;
    } m_descriptors;*/

    const platform::Window& m_window;
};

}