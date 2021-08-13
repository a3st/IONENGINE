// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/api.h"
#include "renderer/frame_graph.h"
#include "platform/base/window.h"
#include "lib/memory.h"

namespace ionengine {

using namespace renderer;
using namespace memory_literals;

class RenderSystem {
public:

    RenderSystem(platform::Window& window) : m_window(window) {

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

        
        
        /*auto& graphics_queue = device->get_command_queue(renderer::CommandListType::Graphics);
        auto fence = device->create_fence(0);
        std::cout << m_window.get().get_window_size().width << " " << m_window.get().get_client_size().height << std::endl;

        std::vector<std::unique_ptr<Shader>> shaders;
        shaders.emplace_back(device->create_shader(renderer::read_shader_code("shaders/pc/basic_vert.bin")));
        shaders.emplace_back(device->create_shader(renderer::read_shader_code("shaders/pc/basic_frag.bin")));

        std::vector<renderer::DescriptorSetLayoutBinding> bindings = {
            { renderer::ShaderType::Vertex, renderer::ViewType::ConstantBuffer, 0, 0, 1 }
        };

        auto layout = device->create_descriptor_set_layout(bindings);

        renderer::RenderPassDesc render_pass_desc = { { { renderer::Format::RGBA8unorm, renderer::RenderPassLoadOp::Load, renderer::RenderPassStoreOp::Store } } };
        auto render_pass = device->create_render_pass(render_pass_desc);

        renderer::GraphicsPipelineDesc pipeline_desc = {
            {
                { *shaders[0], renderer::ShaderType::Vertex },
                { *shaders[1], renderer::ShaderType::Pixel }
            },
            *layout,
            {
                { 0, "POSITION", renderer::Format::RGB32float, sizeof(math::Fvector3) }
            },
            *render_pass
        };

        auto pipeline = device->create_graphics_pipeline(pipeline_desc);

        auto memory_block = device->allocate_memory(MemoryType::Default, 512_mb, 0, ResourceFlags::IndexBuffer);

        auto index_buffer = device->create_buffer(ResourceFlags::IndexBuffer, 24_mb);
        index_buffer->bind_memory(*memory_block, 0);

        std::vector<DescriptorPoolSize> pool_sizes = {
            { ViewType::ConstantBuffer, 2 },
            { ViewType::RenderTarget, 2 }
        };

        auto descriptor_pool = device->create_descriptor_pool(pool_sizes);

        ViewDesc view_desc = { ViewType::RenderTarget, ViewDimension::Texture2D };
        auto view_buffer = device->create_view(*descriptor_pool, m_swapchain.get()->get_back_buffer(m_frame_index), view_desc);

        FrameBufferDesc frame_buffer_desc = { *render_pass, 800, 600, { *view_buffer } };
        auto frame_buffer = device->create_frame_buffer(frame_buffer_desc);

        ClearValueDesc clear_value{};

        auto command_list = device->create_command_list(CommandListType::Graphics);
        command_list->bind_pipeline(*pipeline);
        command_list->set_scissor_rect(0, 0, 800, 600);
        command_list->set_viewport(0, 0, 800, 600);
        command_list->resource_barriers({ { m_swapchain.get()->get_back_buffer(m_frame_index), ResourceState::Present, ResourceState::RenderTarget } });
        command_list->begin_render_pass(*render_pass, *frame_buffer, clear_value);
        command_list->end_render_pass();
        command_list->resource_barriers({ { m_swapchain.get()->get_back_buffer(m_frame_index), ResourceState::RenderTarget, ResourceState::Present } });
        command_list->close();*/
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {
        
    }

private:

    std::reference_wrapper<platform::Window> m_window;

    std::unique_ptr<renderer::Instance> m_instance;
    std::unique_ptr<renderer::Adapter> m_adapter;
    std::unique_ptr<renderer::Device> m_device;
    std::unique_ptr<renderer::Swapchain> m_swapchain;

    std::unique_ptr<renderer::FrameGraph> m_frame_graph;

    uint32 m_swap_buffer_count = 2;

    uint32 m_frame_index = 0;
};

}