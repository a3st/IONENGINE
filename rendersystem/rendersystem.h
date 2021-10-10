// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/engine_system.h"

#include "lib/vector2.h"

#include "lib/memory.h"
#include "gfx/gfx.h"

#include "limits.h"

#include "framegraph.h"

#include "texture_pool.h"

namespace ionengine::rendersystem {

using namespace memory_literals;

class RenderSystem : public EngineSystem {
public:

    RenderSystem(platform::wnd::Window* window) 
        : 
            m_window(window),

            m_device_id(0),
            m_render_size{},
            m_buffered_frames(0) {

        window->set_label(lib::format<char>("IONENGINE - {}", gfx::api_name));

        auto& client_size = m_window->get_client_size();
        m_render_size.x = client_size.width;
        m_render_size.y = client_size.height;

        m_buffered_frames = 2;

        m_device = gfx::create_unique_device(m_device_id, window->get_handle(), m_render_size.x, m_render_size.y, m_buffered_frames, 1);

        m_texture_pool = std::make_unique<TexturePool>(m_device.get(), 1);

        // Initialize frame resources
        m_present_command_lists.resize(m_buffered_frames);
        m_fences.resize(m_buffered_frames);
        m_fence_values.resize(m_buffered_frames);

        for(uint32 i = 0; i < m_buffered_frames; ++i) {
            m_present_command_lists[i] = m_device->create_command_list(gfx::CommandListType::Graphics);
            m_fences[i] = m_device->create_fence(0);
            m_fence_values[i] = 0;
            m_swapchain_textures.emplace_back(std::make_unique<Texture>(m_device.get()))->create_from_swapchain(i).value();
        }

        gfx::AdapterDesc adapter_desc = m_device->get_adapter_desc();
        std::cout << lib::format<char>("Adapter name: {}, Local memory size: {}, Adapter Id: {}, Vendor Id: {}", 
            adapter_desc.name, adapter_desc.local_memory, adapter_desc.device_id, adapter_desc.vendor_id) << std::endl;
        
        m_framegraph = std::make_unique<FrameGraph>(m_device.get());

        gfx::ResourceDesc res_desc{};
        res_desc.dimension = gfx::ViewDimension::Buffer;
        res_desc.width = 1_mb;
        res_desc.height = 1;
        res_desc.mip_levels = 1;
        res_desc.array_size = 1;
        res_desc.flags = gfx::ResourceFlags::VertexBuffer;
        m_basic_resource = m_device->create_resource(gfx::ResourceType::Buffer, gfx::MemoryType::Upload, res_desc);

        std::vector<math::Fvector3> floats = {
            { 0.0f, 0.25f, 0.0f },
            { 0.25f, -0.25f, 0.0f },
            { -0.25f, -0.25f, 0.0f }
        };

        byte* mapped = m_basic_resource->map();
        std::memcpy(mapped, floats.data(), sizeof(math::Fvector3) * floats.size());
        m_basic_resource->unmap();

        gfx::RenderPassDesc render_pass_desc{};
        render_pass_desc.colors.emplace_back(gfx::RenderPassColorDesc { gfx::Format::RGBA8unorm, gfx::RenderPassLoadOp::DontCare, gfx::RenderPassStoreOp::DontCare });
        auto render_pass = m_device->create_render_pass(render_pass_desc);

        std::vector<gfx::BindingSetInputDesc> bindings = {
            { gfx::ShaderType::Vertex, gfx::ViewType::ConstantBuffer, 0, 0, 1 }
        };

        binding_set_layout = m_device->create_binding_set_layout(bindings);
        m_binding_set = m_device->create_binding_set(binding_set_layout.get());

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
        m_basic_pipeline = m_device->create_pipeline(pipeline_desc);

        m_texture_pool->allocate();
    }

    void tick() override {

        const uint32 frame_index = m_device->get_swapchain_buffer_index();

        struct BasicPassData {
            FrameGraphResource* swapchain;
        };

        auto basic_pass = m_framegraph->add_pass<BasicPassData>(
            "BasicPass",
            [&](FrameGraphPassBuilder* builder, BasicPassData& data) {
                data.swapchain = builder->create(FrameGraphResourceType::Attachment, m_swapchain_textures[frame_index].get(), FrameGraphResourceFlags::Swapchain);
                data.swapchain = builder->write(data.swapchain, FrameGraphResourceOp::Clear, { 0.2f, 0.2f, 0.2f, 1.0f });
            },
            [=](FrameGraphPassContext* context, const BasicPassData& data) {
                
                /*

                for(render_queues[Type::Opaque]) {
                    // copy data to index, vertex buffer
                    auto copy_command = context->get_copy_command_f_pool();
                    copy_command->copy_buffers(...)
                }

                */

                context->get_command_list()->set_viewport(0, 0, m_render_size.x, m_render_size.y);
                context->get_command_list()->set_scissor_rect(0, 0, m_render_size.x, m_render_size.y);

                context->get_command_list()->bind_pipeline(m_basic_pipeline.get());
                context->get_command_list()->set_binding_set(m_binding_set.get());

                context->get_command_list()->set_vertex_buffer(0, m_basic_resource.get(), sizeof(math::Fvector3));
                context->get_command_list()->draw_instanced(3, 1, 0, 0);
            }
        );

        m_framegraph->execute(m_present_command_lists[frame_index].get());

        m_device->present();

        // wait for previous frame
        {
            const uint64 prev_fence_value = m_fence_values[frame_index];
            m_device->signal(gfx::CommandListType::Graphics, m_fences[frame_index].get(), m_fence_values[frame_index]);
            m_fence_values[frame_index]++;

            m_fences[frame_index]->wait(prev_fence_value);
        }
    }

    void resize(const uint32 width, const uint32 height) {

        if(m_render_size.x == width || m_render_size.y == height) {
            return;
        }

        m_render_size.x = width;
        m_render_size.y = height;
        
        const uint32 frame_index = m_device->get_swapchain_buffer_index();

        const uint64 prev_fence_value = m_fence_values[frame_index];
        m_device->signal(gfx::CommandListType::Graphics, m_fences[frame_index].get(), m_fence_values[frame_index]);
        m_fence_values[frame_index]++;

        m_fences[frame_index]->wait(prev_fence_value);

        m_swapchain_textures.clear();

        m_device->resize_swapchain_buffers(width, height);

        for(uint32 i = 0; i < m_buffered_frames; ++i) {
            m_swapchain_textures.emplace_back(std::make_unique<Texture>(m_device.get()))->create_from_swapchain(i).value();
        }

        m_framegraph->reset();
    }

private:

    platform::wnd::Window* m_window;

    uint32 m_device_id;
    math::Uvector2 m_render_size; 
    uint32 m_buffered_frames;

    std::unique_ptr<gfx::Device> m_device;

    std::unique_ptr<FrameGraph> m_framegraph;

    std::vector<std::unique_ptr<gfx::CommandList>> m_present_command_lists;
    std::vector<std::unique_ptr<gfx::Fence>> m_fences;
    std::vector<uint64> m_fence_values;
    std::vector<std::unique_ptr<Texture>> m_swapchain_textures;


    //
    std::unique_ptr<gfx::Pipeline> m_basic_pipeline;

    std::unique_ptr<gfx::BindingSet> m_binding_set;

    std::unique_ptr<gfx::BindingSetLayout> binding_set_layout;

    std::unique_ptr<gfx::Resource> m_basic_resource;


    std::unique_ptr<TexturePool> m_texture_pool;

};

}