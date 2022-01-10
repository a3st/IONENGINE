// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    //ImageId image_test = backend->create_image(ImageDimension::_2D, 1024 * 1024 * 512, 1, 1, 1, ImageFormat::RGBA8Unorm, ImageFlags::Color);
    //backend->free_image(image_test);
    //ImageId image_test2 = backend->create_image(ImageDimension::_2D, 1024 * 1024 * 512, 1, 1, 1, ImageFormat::RGBA8Unorm, ImageFlags::Color);

    //ImageId buffer_0 = backend.create_image(ImageType::2D, 800, 600, Flags::Color);
    //ImageViewId rtv_0 = backend.create_image_view(buffer_0, ...);
    //backend.free_image_view(rtv_0);
    //backend.free_image(buffer_0);
    //CommandBufferId cmd_0 = backend.write_cmd(CommandBufferDesc { });
    //FenceId lock_0 = backend.execute(cmd_0);
    //backend.wait(lock_0);
    //backend.present();
}

void WorldRenderer::update() {

    struct BasicPassData {
        FGResourceHandle swapchain;
        FGResourceHandle rtv_0;
    };

    GPUResourceHandle swapchain_handle;

    RenderQueue render_queue;

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
            auto basic_pass_data = _frame_graph.get_data<BasicPassData>(task_1);
            
        },
        [=](FrameGraphContext& context, BasicPassData const& data) {

        }
    );

    _frame_graph.execute(render_queue);

    std::vector<GPUResourceHandle> cmdbuffers;
    cmdbuffers.resize(10);

    JobHandle handle = _thread_pool->push(
        [&]() {
            cmdbuffers[0] = _backend->generate_command_buffer(render_queue);
        }
    );

    _backend->execute_command_buffers(cmdbuffers);

    _backend->swap_buffers();
}