// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/engine_system.h"

#include "lib/memory.h"
#include "gfx/gfx.h"

namespace ionengine {

using namespace memory_literals;

class RenderSystem : public EngineSystem {
public:

    RenderSystem(platform::wnd::Window* window) {

        auto client = window->get_client_size();

        m_device = gfx::create_unique_device(0, window->get_handle(), client.width, client.height, 2, 1);

        gfx::AdapterDesc adapter_desc = m_device->get_adapter_desc();
        std::cout << format<char>("Adapter name: {}, Local memory size: {}, Adapter Id: {}, Vendor Id: {}", 
            adapter_desc.name, adapter_desc.local_memory, adapter_desc.device_id, adapter_desc.vendor_id) << std::endl;

        window->set_label(format<char>("IONENGINE - {}", gfx::api_name));

        std::unique_ptr<gfx::Resource> resources[10];

        for(uint32 i = 0; i < 1; ++i)
        {
            gfx::ResourceDesc res_desc{};
            res_desc.dimension = gfx::ViewDimension::Buffer;
            res_desc.width = 64_kb;
            res_desc.height = 1;
            res_desc.mip_levels = 1;
            res_desc.array_size = 1;
            res_desc.flags = gfx::ResourceFlags::ConstantBuffer;
            resources[i] = m_device->create_resource(gfx::ResourceType::Buffer, gfx::MemoryType::Default, res_desc);
        }

        gfx::ViewDesc view_desc{};
        view_desc.buffer_size = resources[0]->get_desc().width;
        auto view = m_device->create_view(gfx::ViewType::ConstantBuffer, resources[0].get(), view_desc);

        std::vector<gfx::BindingSetBinding> bindings = {
            { gfx::ShaderType::Vertex, gfx::ViewType::ConstantBuffer, 0, 0, 1 }
        };

        auto binding_set_layout = m_device->create_binding_set_layout(bindings);
        auto binding_set = m_device->create_binding_set(binding_set_layout.get());

        gfx::WriteBindingSet write = {
            0,
            1,
            gfx::ViewType::ConstantBuffer,
            { { view.get() } }
        };

        binding_set->write(write);

        gfx::RenderPassDesc render_pass_desc{};
        auto render_pass = m_device->create_render_pass(render_pass_desc);

        gfx::GraphicsPipelineDesc pipeline_desc{};
        pipeline_desc.render_pass = render_pass.get();
        pipeline_desc.layout = binding_set_layout.get();
        pipeline_desc.shaders = { { gfx::ShaderType::Vertex, "s.bin" } };
        auto pipeline = m_device->create_pipeline(pipeline_desc);
    }

    void tick() override {

    }

    void resize(const uint32 width, const uint32 height) {
        
    }

private:

    std::unique_ptr<gfx::Device> m_device;
};

}


/*#include "renderer/api/api.h"
#include "renderer/fg/frame_graph.h"
#include "renderer/quad_renderer.h"

#include "lib/memory.h"

namespace ionengine {

using namespace memory_literals;

class RenderSystem : public EngineSystem {
public:

    RenderSystem(platform::wnd::Window* window) : m_window(window), m_buffer_count(2) {

        std::cout << format<char>("RenderSystem ({} API) initialized", renderer::api::get_api_name()) << std::endl;
        m_instance = renderer::api::create_unique_instance();

        window->set_label(format<char>("IONENGINE - {}", renderer::api::get_api_name()));

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

        auto client_size = m_window->get_client_size();
        m_swapchain = m_device->create_swapchain(m_window->get_handle(), client_size.width, client_size.height, m_buffer_count);

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

    platform::wnd::Window* m_window;

    std::unique_ptr<renderer::api::Instance> m_instance;
    std::unique_ptr<renderer::api::Adapter> m_adapter;
    std::unique_ptr<renderer::api::Device> m_device;
    std::unique_ptr<renderer::api::Swapchain> m_swapchain;
    std::unique_ptr<renderer::api::Fence> m_fence;

    std::vector<uint64> m_fence_values;

    uint32 m_buffer_count;

    std::unique_ptr<renderer::BaseRenderer> m_renderer;
};
*/