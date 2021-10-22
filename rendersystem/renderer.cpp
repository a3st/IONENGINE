// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : window_(window) {

    device_ = lgfx::Device(0, window->GetNativeHandle(), 800, 600, 2, 1);

    // Swapchain Data
    frame_descriptor_pool_ = lgfx::DescriptorPool(&device_, 2, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone);

    frame_resources_.resize(2);
    lgfx::TextureViewDesc view_desc{};
    view_desc.dimension = lgfx::Dimension::kTexture2D;

    for(uint32_t i = 0; i < 2; ++i) {
        frame_resources_[i].texture = lgfx::Texture(&device_, i);
        frame_resources_[i].view = lgfx::TextureView(&device_, &frame_descriptor_pool_, &frame_resources_[i].texture, view_desc);
    }
}

void Renderer::Frame() {

    uint32_t frame_index = 0;

    struct BasicPass {
        FrameGraphResource swapchain;
    };

    auto data_pass = frame_graph_.AddTask<BasicPass>(FrameGraphTaskType::RenderPass,
        [&](FrameGraphTaskBuilder* builder, BasicPass& data) {
            data.swapchain = builder->Create(FrameGraphResourceType::Attachment, 
                FrameGraphExternalResourceInfo { 
                    FrameGraphExternalResourceInfo::Attachment { &frame_resources_[frame_index].texture, &frame_resources_[frame_index].view } 
                });
            
            builder->Write(&data.swapchain, FrameGraphResourceWriteOp::Clear, Color { 0.4f, 0.5f, 0.3f, 1.0f });
        },
        [=](FrameGraphTaskContext* context, BasicPass& data) {
            
        });

    frame_graph_.Execute();
}