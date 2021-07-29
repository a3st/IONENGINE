// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/api.h"
#include "platform/window.h"

namespace ionengine {

class RenderSystem final {
public:

    RenderSystem(platform::Window& window) : 
        m_window(window), m_swapchain(m_device, m_window.get().get_handle(), 800, 600, 2)  {

        auto config = m_device.get_adapter_config();

        std::cout <<
            format<std::string>("Device Id: {}\nDevice Name: {}\nMemory Device: {}", 
                config.device_id, 
                config.device_name, 
                config.dedicated_memory) <<
        std::endl;

        std::cout << m_swapchain.get_framebuffer_attachments().size() << std::endl;

        //std::vector<renderer::Shader> shaders;
        //shaders.emplace_back(m_device, renderer::ShaderType::Vertex, "triangle_01.vert.spv");
        //shaders.emplace_back(m_device, renderer::ShaderType::Fragment, "triangle_01.frag.spv");

        /*renderer::PipelineConfig pipeline_config = { shaders };

        renderer::Pipeline pipeline(m_device, pipeline_config);*/
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {
        
    }

private:

    std::reference_wrapper<platform::Window> m_window;

    renderer::Device m_device;
    renderer::Swapchain m_swapchain;
};

}