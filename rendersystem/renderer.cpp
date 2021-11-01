// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : 
    window_(window),
    device_(0, window->GetNativeHandle(), 800, 600, 2, 1),
    frame_graph_(&device_),
    frame_descriptor_pool_(&device_, 2, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone),
    test_descriptor_pool(&device_, 2, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kNone),
    test_memory_pool(&device_, 1024, 0, lgfx::MemoryType::kUpload, lgfx::MemoryFlags::kBuffers),
    pipeline_cache_(&device_) {

    frame_resources_.textures.resize(2);
    frame_resources_.texture_views.resize(2);
    frame_resources_.fences.resize(2);
    frame_resources_.fence_values.resize(2);

    for(uint32_t i = 0; i < 2; ++i) {
        frame_resources_.textures[i] = std::make_unique<lgfx::Texture>(&device_, i);
        frame_resources_.texture_views[i] = std::make_unique<lgfx::TextureView>(&device_, &frame_descriptor_pool_, frame_resources_.textures[i].get(), lgfx::Dimension::kTexture2D, 0, 0, 0, 0);
        frame_resources_.fences[i] = std::make_unique<lgfx::Fence>(&device_, 0);
    }

    // Main DescriptorLayout
    {
        std::vector<lgfx::DescriptorLayoutBinding> bindings = {
            { lgfx::DescriptorType::kConstantBuffer, 0, 0, 3, lgfx::ShaderType::kVertex }
        };

        descriptor_layouts_.emplace_back(std::make_unique<lgfx::DescriptorLayout>(&device_, bindings));
    }

    std::vector<lgfx::InputLayoutDesc> inputs = {
        { "POSITION", 0, lgfx::Format::kRGB32float, 0, 0 }
    };

    shaders_.emplace_back(std::make_unique<lgfx::Shader>(&device_, lgfx::ShaderType::kVertex, "shaders/basic_vert.bin"));
    shaders_.emplace_back(std::make_unique<lgfx::Shader>(&device_, lgfx::ShaderType::kPixel, "shaders/basic_frag.bin"));

    descriptor_set_ = std::make_unique<lgfx::DescriptorSet>(&device_, descriptor_layouts_[0].get());

    buffer_ = std::make_unique<lgfx::Buffer>(&device_, &test_memory_pool, 1024 * 1024, lgfx::BufferFlags::kVertexBuffer);
    buffer_view_ = std::make_unique<lgfx::BufferView>(&device_, nullptr, buffer_.get(), lgfx::Format::kUnknown, static_cast<uint32_t>(sizeof(float) * 3));

    std::byte* data = buffer_->Map();
    std::vector<float> triangle_data = {
        0.0f, 0.25f, 0.0f,
        0.25f, -0.25f, 0.0f,
        -0.25f, -0.25f, 0.0f
    };
    std::memcpy(data, triangle_data.data(), triangle_data.size() * sizeof(float));
    buffer_->Unmap();
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
                frame_resources_.textures[frame_index_].get(), frame_resources_.texture_views[frame_index_].get(), true });

            /*FrameGraphResource* dummy = builder->Create(FrameGraphResourceDesc {
                FrameGraphResourceType::kAttachment,
                800, 600, 1, 1,
                lgfx::Format::kRGBA8unorm,
                lgfx::TextureFlags::kRenderTarget });*/

            builder->Write(swapchain, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.3f, 1.0f });
            //builder->Write(dummy, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.9f, 1.0f });
        },
        [=](FrameGraphContext* context) {
            
            context->GetBuffer()->SetViewport(0, 0, 800, 600);
            context->GetBuffer()->SetScissorRect(0, 0, 800, 600);

            /*lgfx::PipelineDesc pipeline_desc{};
            pipeline_desc.type = lgfx::PipelineType::kGraphics;
            pipeline_desc.layout = descriptor_layouts_[0].get();
            shaders_c_.emplace_back(shaders_[0].get());
            shaders_c_.emplace_back(shaders_[1].get());
            pipeline_desc.shaders = shaders_c_;
            pipeline_desc.render_pass = context->GetRenderPass();
            lgfx::Pipeline* pipeline = pipeline_cache_.GetPipeline(pipeline_desc);*/

            //context->GetBuffer()->BindPipeline(nullptr);
            context->GetBuffer()->BindDescriptorSet(descriptor_set_.get());

            context->GetBuffer()->SetVertexBuffer(0, buffer_view_.get());
            context->GetBuffer()->DrawInstanced(3, 1, 0, 0);

            shaders_c_.clear();
            
        });

    frame_graph_.Execute();

    EndFrame();
}

void Renderer::EndFrame() {

    const uint64_t fence_value = frame_resources_.fence_values[frame_index_];
    device_.Signal(lgfx::CommandBufferType::kGraphics, frame_resources_.fences[frame_index_].get(), fence_value);
    ++frame_resources_.fence_values[frame_index_];
    frame_resources_.fences[frame_index_]->Wait(fence_value);
}

void Renderer::Resize(const uint32_t width, const uint32_t height) {

}