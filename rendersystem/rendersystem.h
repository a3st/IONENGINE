// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/engine_system.h"

#include "lib/memory.h"
#include "gfx/gfx.h"

#include "limits.h"

#include "frame_buffer_cache.h"
#include "render_pass_cache.h"
#include "pipeline_cache.h"

#include "texture_pool.h"
#include "buffer.h"

#include "framegraph.h"

namespace ionengine::rendersystem {

using namespace memory_literals;

class RenderSystem : public EngineSystem {
public:

    RenderSystem(platform::wnd::Window* window) {

        auto client = window->get_client_size();

        m_device = gfx::create_unique_device(0, window->get_handle(), client.width, client.height, 2, 1);

        gfx::AdapterDesc adapter_desc = m_device->get_adapter_desc();
        std::cout << lib::format<char>("Adapter name: {}, Local memory size: {}, Adapter Id: {}, Vendor Id: {}", 
            adapter_desc.name, adapter_desc.local_memory, adapter_desc.device_id, adapter_desc.vendor_id) << std::endl;

        window->set_label(lib::format<char>("IONENGINE - {}", gfx::api_name));
        
        m_framegraph = std::make_unique<FrameGraph>(m_device.get());
        

        /*std::unique_ptr<gfx::Resource> resources[10];

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
        pipeline_desc.vertex_inputs = {
            { "POSITION", 0, gfx::Format::RGB32float, 0, 0 }
        };
        pipeline_desc.render_pass = render_pass.get();
        pipeline_desc.layout = binding_set_layout.get();
        pipeline_desc.shaders = { 
            { gfx::ShaderType::Vertex, "shaders/pc/basic_vert.bin" },
            { gfx::ShaderType::Pixel, "shaders/pc/basic_frag.bin" },
        };
        auto pipeline = m_device->create_pipeline(pipeline_desc);*/
    }

    void tick() override {

        struct BasicPassData {
            FrameGraphResource* swapchain;
        };

        auto basic_pass = m_framegraph->add_pass<BasicPassData>(
            "BasicPass", 
            [&](RenderPassBuilder* builder, BasicPassData& data) {
                data.swapchain = builder->create(FrameGraphResourceType::Attachment, Texture::Format::RGBA8, 800, 600, FrameGraphResourceFlags::Swapchain);
                data.swapchain = builder->write(data.swapchain, FrameGraphResourceOp::Clear, { 0.5f, 0.4f, 0.3f, 1.0f });
            },
            [=](RenderPassContext* context, const BasicPassData& data) {

            }
        );

        struct FinalPassData {
            FrameGraphResource* output;
        };

        auto final_pass = m_framegraph->add_pass<FinalPassData>(
            "FinalPass", 
            [&](RenderPassBuilder* builder, FinalPassData& data) {
                data.output = builder->write(basic_pass.get_data().swapchain, FrameGraphResourceOp::Clear, {0.5, 0.6f, 0.4f, 1.0f });
            },
            [=](RenderPassContext* context, const FinalPassData& data) {

            }
        );

        m_framegraph->execute();
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

private:

    std::unique_ptr<gfx::Device> m_device;
    std::unique_ptr<FrameGraph> m_framegraph;
};

}