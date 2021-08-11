// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/api.h"
#include "platform/base/window.h"
#include "lib/memory.h"

namespace ionengine {

using namespace renderer;
using namespace memory;

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
        auto& graphics_queue = device->get_command_queue(renderer::CommandListType::Graphics);
        m_swapchain = device->create_swapchain(m_window.get().get_native_handle(), m_window.get().get_window_size().width, m_window.get().get_window_size().height, 2);
        auto fence = device->create_fence(0);

        std::vector<std::unique_ptr<renderer::Shader>> shaders = {
            device->create_shader(renderer::read_shader_code("shaders/pc/basic_vert.bin")),
            device->create_shader(renderer::read_shader_code("shaders/pc/basic_frag.bin"))
        };

        std::vector<renderer::DescriptorSetLayoutBinding> bindings = {
            { renderer::ShaderType::Vertex, renderer::ViewType::ConstantBuffer, 0, 0, 1 }
        };

        auto layout = device->create_descriptor_set_layout(bindings);

        renderer::RenderPassDesc render_pass_desc = { { { renderer::Format::R8G8B8A8unorm, renderer::RenderPassLoadOp::Load, renderer::RenderPassStoreOp::Store } } };
        auto render_pass = device->create_render_pass(render_pass_desc);

        renderer::GraphicsPipelineDesc pipeline_desc = {
            {
                { *shaders[0], renderer::ShaderType::Vertex },
                { *shaders[1], renderer::ShaderType::Pixel }
            },
            *layout,
            {
                { 0, "POSITION", renderer::Format::R32G32B32float, sizeof(math::Fvector3) }
            },
            *render_pass
        };

        auto pipeline = device->create_graphics_pipeline(pipeline_desc);

        auto memory_block = device->allocate_memory(MemoryType::Default, 512_mb, 0, ResourceFlags::IndexBuffer);

        auto index_buffer = device->create_buffer(ResourceFlags::IndexBuffer, 24_mb);
        index_buffer->bind_memory(*memory_block, 0);

        std::vector<DescriptorPoolSize> pool_sizes = {
            { ViewType::ConstantBuffer, 2 }
        };

        auto descriptor_pool = device->create_descriptor_pool(pool_sizes);

        auto command_list = device->create_command_list(CommandListType::Graphics);
        
        command_list->bind_pipeline(*pipeline);
        command_list->close();


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