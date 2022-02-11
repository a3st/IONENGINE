// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {


}

void WorldRenderer::update() {

    

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