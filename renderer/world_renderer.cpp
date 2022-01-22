// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    cmdbuffs.push_back(backend->create_command_buffer(QueueType::Graphics));
}

void WorldRenderer::update() {

    GPUResourceHandle swapchain_handle = _backend->get_swap_texture();

    // Command reset
    {
        CommandGenerator generator = _backend->create_command_generator(cmdbuffs[0]);
        generator
            .set_viewport(0, 0, 800, 600)
            .set_scissor(0, 0, 800, 600)
            .barrier(swapchain_handle, MemoryState::Present, MemoryState::RenderTarget)
            .begin_render_pass(1, 
                { swapchain_handle }, 
                { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
                { Color(0.6f, 0.2f, 0.4f, 1.0f) },
                GPUResourceHandle(),
                {},
                { 0.0f, 0x0 }
            )
            .end_render_pass()
            .barrier(swapchain_handle, MemoryState::RenderTarget, MemoryState::Present);
    } // Command close

    _backend->execute_command_buffers(QueueType::Graphics, { cmdbuffs[0] });

    _backend->swap_buffers();

    /*struct BasicPassData {
        FGResourceHandle swapchain;
        FGResourceHandle rtv_0;
    };

    GPUResourceHandle swapchain_handle;

    FGTaskHandle task_1 = _frame_graph.add_task<BasicPassData>(
        FGTaskType::RenderPass,
        [&](FrameGraphBuilder& builder, BasicPassData& data) {
            data.swapchain = builder.create(FGResourceType::SwapchainAttachment, swapchain_handle);
        },
        [=](FrameGraphContext& context, BasicPassData const& data) {
            
        }
    );

    FGTaskHandle task_2 = _frame_graph.add_task<BasicPassData>(
        FGTaskType::AsyncComputePass,
        [&](FrameGraphBuilder& builder, BasicPassData& data) {
            
        },
        [=](FrameGraphContext& context, BasicPassData const& data) {

        }
    );

    _frame_graph.wait_until(task_2);

    FGTaskHandle task_3 = _frame_graph.add_task<BasicPassData>(
        FGTaskType::RenderPass,
        [&](FrameGraphBuilder& builder, BasicPassData& data) {
            //auto basic_pass_data = _frame_graph.get_data<BasicPassData>(task_1);
            
        },
        [=](FrameGraphContext& context, BasicPassData const& data) {

        }
    );

    _frame_graph.execute();*/
}