// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

#include "../lmath/lmath.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : 
    window_(window),
    device_(0, window->GetNativeHandle(), 800, 600, 2, 1),
    frame_graph_(&device_),
    frame_descriptor_pool_(&device_, 2, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone),
    test_descriptor_pool(&device_, 2, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kNone),
    test_memory_pool(&device_, 1024, 0, lgfx::MemoryType::kUpload, lgfx::MemoryFlags::kBuffers),
    pipeline_cache_(&device_),
    buffer_cache_(&device_),
    buffer_view_cache_(&device_) {

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

    inputs_ = {
        { "POSITION", 0, lgfx::Format::kRGB32float, 0, 0 }
    };

    shaders_.emplace_back(std::make_unique<lgfx::Shader>(&device_, lgfx::ShaderType::kVertex, "shaders/basic_vert.bin"));
    shaders_.emplace_back(std::make_unique<lgfx::Shader>(&device_, lgfx::ShaderType::kPixel, "shaders/basic_frag.bin"));

    shaders_c_.emplace_back(shaders_[0].get());
    shaders_c_.emplace_back(shaders_[1].get());

    descriptor_set_ = std::make_unique<lgfx::DescriptorSet>(&device_, descriptor_layouts_[0].get());

    stage_buffer_ = std::make_unique<lgfx::Buffer>(&device_, &test_memory_pool, kRendererStageBufferSize, lgfx::BufferFlags::kCopySource);
}

void Renderer::BeginFrame() {

    frame_index_ = device_.GetSwapchainBufferIndex();
}

void Renderer::Frame() {

    BeginFrame();

    frame_graph_.command_buffer_->Reset();

    struct WorldMatrix {

        lmath::Matrixf model;
        lmath::Matrixf view;
        lmath::Matrixf proj;
    };

    lgfx::Buffer* worldmatrix_buff = buffer_cache_.GetBuffer(256, lgfx::BufferFlags::kConstantBuffer);
    lgfx::BufferView* worldmatrix_view = buffer_view_cache_.GetBufferView(worldmatrix_buff, lgfx::Format::kUnknown, 0);

    lgfx::Buffer* vertex_buff = buffer_cache_.GetBuffer(1024 * 1024, lgfx::BufferFlags::kVertexBuffer);
    lgfx::BufferView* vertex_buff_view = buffer_view_cache_.GetBufferView(vertex_buff, lgfx::Format::kUnknown, sizeof(float) * 3);

    triangle_data_.emplace_back(0.0f);
    triangle_data_.emplace_back(0.25f);
    triangle_data_.emplace_back(0.0f);
    triangle_data_.emplace_back(0.25f);
    triangle_data_.emplace_back(-0.25f);
    triangle_data_.emplace_back(0.0f);
    triangle_data_.emplace_back(-0.25f);
    triangle_data_.emplace_back(-0.25f);
    triangle_data_.emplace_back(0.0f);

    WorldMatrix world{};
    auto model_ = lmath::Matrixf::Translate(lmath::Vector3f(0.5f, -0.5f, 0.0f));
    auto view_ = lmath::Matrixf::Identity();
    auto proj_ = lmath::Matrixf::Identity();
    model_.Transpose();
    view_.Transpose();
    proj_.Transpose();

    world.model = model_;
    world.view = view_;
    world.proj = proj_;

    std::byte* data = stage_buffer_->Map();
    std::memcpy(data, triangle_data_.data(), triangle_data_.size() * sizeof(float));
    std::memcpy(data + triangle_data_.size() * sizeof(float), &world, sizeof(WorldMatrix));
    stage_buffer_->Unmap();

    frame_graph_.command_buffer_->BufferMemoryBarrier(vertex_buff, lgfx::MemoryState::kCommon, lgfx::MemoryState::kCopyDest);
    frame_graph_.command_buffer_->CopyBuffer(vertex_buff, 0, stage_buffer_.get(), 0, triangle_data_.size() * sizeof(float));
    frame_graph_.command_buffer_->BufferMemoryBarrier(vertex_buff, lgfx::MemoryState::kCopyDest, lgfx::MemoryState::kCommon);

    frame_graph_.command_buffer_->BufferMemoryBarrier(worldmatrix_buff, lgfx::MemoryState::kCommon, lgfx::MemoryState::kCopyDest);
    frame_graph_.command_buffer_->CopyBuffer(worldmatrix_buff, 0, stage_buffer_.get(), triangle_data_.size() * sizeof(float), sizeof(WorldMatrix));
    frame_graph_.command_buffer_->BufferMemoryBarrier(worldmatrix_buff, lgfx::MemoryState::kCopyDest, lgfx::MemoryState::kCommon);

    struct CopyPassData {
        FrameGraphResource resource;
    };

    frame_graph_.AddTask2<CopyPassData>(
        FrameGraphTaskType::kCopyPass,
        [&](FrameGraphBuilder* builder, CopyPassData& data) {

            
        },
        [=](FrameGraphContext* context, CopyPassData& data) {
            
            
        });

    FrameGraphTask* basic_task = frame_graph_.AddTask(FrameGraphTaskType::kRenderPass,
        [&](FrameGraphBuilder* builder) {

            FrameGraphResource* swapchain = builder->Create(FrameGraphResourceDesc {
                FrameGraphResourceType::kAttachment,
                0, 0, 0, 0, 
                lgfx::Format::kUnknown, 
                lgfx::TextureFlags::kRenderTarget,
                frame_resources_.textures[frame_index_].get(), frame_resources_.texture_views[frame_index_].get(), true });

            builder->Write(swapchain, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.3f, 1.0f });
        },
        [=](FrameGraphContext* context) {
            
            context->GetBuffer()->SetViewport(0, 0, 800, 600);
            context->GetBuffer()->SetScissorRect(0, 0, 800, 600);

            lgfx::Pipeline* pipeline = pipeline_cache_.GetPipeline(
                lgfx::PipelineType::kGraphics, 
                descriptor_layouts_[0].get(), 
                inputs_, 
                shaders_c_, 
                {}, {}, {}, 
                context->GetRenderPass());

            descriptor_set_->WriteBuffer(0, 0, worldmatrix_view);

            context->GetBuffer()->BindPipeline(pipeline);
            context->GetBuffer()->BindDescriptorSet(descriptor_set_.get());

            context->GetBuffer()->SetVertexBuffer(0, vertex_buff_view);
            context->GetBuffer()->DrawInstanced(3, 1, 0, 0);
        });

    frame_graph_.Execute();

    EndFrame();

    buffer_cache_.Clear();
    triangle_data_.clear();
}

void Renderer::EndFrame() {

    const uint64_t fence_value = frame_resources_.fence_values[frame_index_];
    device_.Signal(lgfx::CommandBufferType::kGraphics, frame_resources_.fences[frame_index_].get(), fence_value);
    ++frame_resources_.fence_values[frame_index_];
    frame_resources_.fences[frame_index_]->Wait(fence_value);
}

void Renderer::Resize(const uint32_t width, const uint32_t height) {

}