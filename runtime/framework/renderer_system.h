// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/api.h"
#include "platform/base/window.h"

namespace ionengine {

class RenderSystem {
public:

    RenderSystem(platform::Window& window) : m_window(window) {

        m_instance = renderer::create_unique_instance();
        auto adapters = m_instance->enumerate_adapters();

        for(uint32 i = 0; i < adapters.size(); ++i) {
            std::cout <<
                format<std::string>("Vendor Id: {}\nDevice Id: {}\nDevice Name: {}\nDedicated Memory: {}\n",
                    adapters[i]->get_vendor_id(), 
                    adapters[i]->get_device_id(), 
                    adapters[i]->get_name(), 
                    adapters[i]->get_memory()
                ) <<
            std::endl;
        }

        auto adapter = std::move(adapters[0]);
        auto device = adapter->create_device();
        auto& queue = device->get_command_queue(renderer::CommandListType::Graphics);

        m_swapchain = device->create_swapchain(m_window.get().get_native_handle(), m_window.get().get_window_size().width, m_window.get().get_window_size().height, 2);

        std::vector<std::unique_ptr<renderer::Shader>> shaders;
#ifdef RENDERER_API_D3D12
        shaders.emplace_back(device->create_shader(renderer::read_shader_code("shaders/pc/basic_vert.bin")));
        shaders.emplace_back(device->create_shader(renderer::read_shader_code("shaders/pc/basic_frag.bin")));
#else
        shaders.emplace_back(device->create_shader(renderer::read_shader_code("shaders/vk/basic_vert.bin")));
        shaders.emplace_back(device->create_shader(renderer::read_shader_code("shaders/vk/basic_frag.bin")));
#endif

        std::vector<renderer::DescriptorSetLayoutBinding> bindings = {
            { renderer::ShaderType::Vertex, renderer::ViewType::ConstantBuffer, 0, 0, 1 }
        };

        auto layout = device->create_descriptor_set_layout(bindings);

        renderer::RenderPassDesc render_pass_desc = { { { renderer::Format::Test, renderer::RenderPassLoadOp::Load, renderer::RenderPassStoreOp::Store } } };
        auto render_pass = device->create_render_pass(render_pass_desc);

        renderer::GraphicsPipelineDesc pipeline_desc = {
            {
                { *shaders[0], renderer::ShaderType::Vertex },
                { *shaders[1], renderer::ShaderType::Pixel }
            },
            *layout,
            {
                { 0, "POSITION", renderer::Format::Test, sizeof(math::Fvector3) }
            },
            *render_pass
        };

        auto pipeline = device->create_graphics_pipeline(pipeline_desc);
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {
        
    }

private:

    std::reference_wrapper<platform::Window> m_window;

    std::unique_ptr<renderer::Instance> m_instance;
    std::unique_ptr<renderer::Swapchain> m_swapchain;
};

}