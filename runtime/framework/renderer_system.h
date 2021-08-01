// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/api.h"
#include "platform/window.h"

namespace ionengine {

class RenderSystem {
public:

    RenderSystem(platform::Window& window) : 
        m_window(window),
        m_swapchain(m_device, m_window.get().get_handle(), 800, 600, 2)  {

        auto config = m_device.get_adapter_config();

        std::cout <<
            format<std::string>("Vendor Id: {}\nDevice Id: {}\nDevice Name: {}\nDedicated Memory: {}",
                config.vendor_id, 
                config.device_id, 
                config.device_name, 
                config.dedicated_memory
            ) <<
        std::endl;

        std::vector<renderer::Shader> shaders;
#ifdef RENDERER_API_D3D12
        shaders.emplace_back(m_device, renderer::shader::read_shader_code("shaders/pc/basic_vert.bin"));
        shaders.emplace_back(m_device, renderer::shader::read_shader_code("shaders/pc/basic_frag.bin"));
#else
        shaders.emplace_back(m_device, renderer::shader::read_shader_code("shaders/vk/basic_vert.bin"));
        shaders.emplace_back(m_device, renderer::shader::read_shader_code("shaders/vk/basic_frag.bin"));
#endif

        std::vector<renderer::VertexInputDesc> input_descs = {
            renderer::VertexInputDesc { 0, 0, renderer::FormatType::R32G32B32Float, 8, renderer::VertexInputRateType::Vertex }
        };

        renderer::PipelineConfig pipeline_config{};

        pipeline_config
            .set_primitive_topology(renderer::PrimitiveTopologyType::Triangle)
            .set_vertex_input_desc(input_descs)
            .set_vertex_shader(shaders[0])
            .set_frag_shader(shaders[1]);

        renderer::Pipeline pipeline(m_device, pipeline_config);
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