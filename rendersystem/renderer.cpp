// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : 
    window_(window),
    device_(0, window->GetNativeHandle(), 800, 600, 2, 1),
    frame_graph_(&device_),
    frame_descriptor_pool_(&device_, 2, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone) {

    frame_resources_.textures.resize(2);
    frame_resources_.texture_views.resize(2);
    frame_resources_.fences.resize(2);
    frame_resources_.fence_values.resize(2);

    lgfx::TextureViewDesc view_desc{};
    view_desc.dimension = lgfx::Dimension::kTexture2D;

    for(uint32_t i = 0; i < 2; ++i) {
        frame_resources_.textures[i] = std::make_unique<lgfx::Texture>(&device_, i);
        frame_resources_.texture_views[i] = std::make_unique<lgfx::TextureView>(&device_, &frame_descriptor_pool_, frame_resources_.textures[i].get(), view_desc);
        frame_resources_.fences[i] = std::make_unique<lgfx::Fence>(&device_, 0);
    }
}

void Renderer::BeginFrame() {

    frame_index_ = device_.GetSwapchainBufferIndex();
}

void Renderer::Frame() {

    BeginFrame();

    FrameGraphTask* basic_task = frame_graph_.AddTask(FrameGraphTaskType::kRenderPass,
        [&](FrameGraphBuilder* builder) {

            FrameGraphResource* swapchain = builder->Create(FrameGraphResourceDesc {
                FrameGraphResourceType::kAttachment,
                0, 0, 0, 0, 
                lgfx::Format::kUnknown, 
                lgfx::TextureFlags::kRenderTarget,
                frame_resources_.textures[frame_index_].get(), frame_resources_.texture_views[frame_index_].get() });

            FrameGraphResource* dummy = builder->Create(FrameGraphResourceDesc {
                FrameGraphResourceType::kAttachment,
                800, 600, 1, 1,
                lgfx::Format::kRGBA8unorm,
                lgfx::TextureFlags::kRenderTarget });

            //builder->Write(swapchain, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.3f, 1.0f });
            //builder->Write(dummy, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.9f, 1.0f });
        },
        [=](FrameGraphContext* context) {
            
        });

    frame_graph_.Execute();

    EndFrame();
}

void Renderer::EndFrame() {

    // Wait Previous Frame
    /*const uint64_t fence_value = frame_resources_.fence_values[frame_index_];

    device_->Signal(lgfx::CommandBufferType::kGraphics, frame_resources_.fences[frame_index_].get(), fence_value);

    ++frame_resources_.fence_values[frame_index_];

    frame_resources_.fences[frame_index_]->Wait(fence_value);

    frame_graph_->Flush();*/
}

void Renderer::Resize(const uint32_t width, const uint32_t height) {

}