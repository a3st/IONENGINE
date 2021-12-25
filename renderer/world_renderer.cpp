// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend) : 
    backend_(backend), fg_(*backend) {

    buffer = Buffer(*backend_, BufferType::Vertex, 1024 * 1024);
    memory = Memory(*backend_, MemoryType::Upload, 1024 * 1024 * 512);
    memory.bind_buffer(buffer, 0);
    char8_t* data = memory.map();
    memset(data, 1, sizeof(int));
    memory.unmap();

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
        FrameGraphResourceId swapchain;
    };

    fg_.add_task<BasicPassData>(
        FrameGraphTaskType::RenderPass,
        [&](FrameGraphBuilder& builder, BasicPassData& data) {
            data.swapchain = builder.create(FrameGraphResource { 
                    FrameGraphResourceType::Attachment,
                    800,
                    600,
                    FrameGraphResourceFlags::Presentable
                }
            );

            builder.write(data.swapchain, FrameGraphResourceOp::Clear, FrameGraphResourceClearDesc { Color(1.0f, 0.5f, 0.2f, 1.0f) });
        },
        [=](FrameGraphContext& context, BasicPassData const& data) {

        }
    );

    fg_.execute();
}