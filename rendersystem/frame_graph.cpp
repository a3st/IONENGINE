// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_graph.h"

using namespace ionengine::rendersystem;

lgfx::RenderPassLoadOp FrameGraphBuilder::FrameGraphResourceWriteOpTo(const FrameGraphResourceWriteOp op) {

    switch(op) {
        case FrameGraphResourceWriteOp::kLoad: return lgfx::RenderPassLoadOp::kLoad;
        case FrameGraphResourceWriteOp::kClear: return lgfx::RenderPassLoadOp::kClear;
        default: assert(false && "passed invalid argument to FrameGraphResourceWriteOpTo"); return lgfx::RenderPassLoadOp::kDontCare;
    }
}

FrameGraphContext::FrameGraphContext() {
    
}

FrameGraphBuilder::FrameGraphBuilder(TextureCache* texture_cache, TextureViewCache* texture_view_cache, const uint32_t task_index, std::vector<FrameGraphTaskDesc>& tasks, std::vector<FrameGraphResourceDesc>& resources) : 
    texture_cache_(texture_cache), texture_view_cache_(texture_view_cache), task_index_(task_index), tasks_(tasks), resources_(resources) {

}

FrameGraphResourceId FrameGraphBuilder::Create(const FrameGraphResourceCreateInfo& create_info) {

    auto& task = tasks_[task_index_];

    if(create_info.texture && create_info.texture_view) {
        resources_.emplace_back(FrameGraphResourceDesc { create_info.type, create_info.texture, create_info.texture_view });
    } else {
        lgfx::TextureDesc desc{};
        desc.dimension = lgfx::Dimension::kTexture2D;
        desc.width = create_info.width;
        desc.height = create_info.height;
        desc.array_layers = 1;
        desc.mip_levels = 1;
        desc.format = lgfx::Format::kRGBA8unorm;
        desc.flags = lgfx::TextureFlags::kRenderTarget;
        lgfx::Texture* texture = texture_cache_->GetTexture(desc);

        lgfx::TextureViewDesc view_desc{};
        view_desc.dimension = desc.dimension;
        view_desc.mip_level_count = desc.mip_levels;
         view_desc.array_layer_count = desc.array_layers;
        lgfx::TextureView* texture_view = texture_view_cache_->GetTextureView({ texture, view_desc });

        resources_.emplace_back(FrameGraphResourceDesc { create_info.type, create_info.texture, create_info.texture_view });
    }
    return FrameGraphResourceId(static_cast<uint32_t>(resources_.size() - 1));
}

void FrameGraphBuilder::Read(const FrameGraphResourceId& resource_id) {

}

void FrameGraphBuilder::Write(const FrameGraphResourceId& resource_id, const FrameGraphResourceWriteOp write_op, const Color& clear_color) {

    auto& task = tasks_[task_index_];
    auto& resource = resources_[resource_id.id_];

    if(resource.texture->GetDesc().flags & lgfx::TextureFlags::kDepthStencil) {
        task.frame_buffer_desc.depth_stencil = resource.texture_view;
    } else {
        //task.frame_buffer_desc.colors.emplace_back(resource.texture_view);
        //task.render_pass_desc.colors.emplace_back(lgfx::RenderPassColorDesc { resource.texture->GetDesc().format, to_resource_write_op(write_op), lgfx::RenderPassStoreOp::kStore });
        //task.clear_desc.colors.emplace_back(lgfx::ClearValueColor { clear_color.r, clear_color.g, clear_color.b, clear_color.a });
    }

    task.write_resources.emplace_back(resource_id);
}

FrameGraph::FrameGraph(lgfx::Device* device) : device_(device), task_index_(0) {

    command_buffer_ = std::make_unique<lgfx::CommandBuffer>(device, lgfx::CommandBufferType::kGraphics);
    tasks_.resize(kFrameGraphTaskSize);
//    resources_.reserve(kFrameGraphResourceSize);

    command_buffer_->Reset();

    texture_cache_ = std::make_unique<TextureCache>(device);
    texture_view_cache_ = std::make_unique<TextureViewCache>(device);
    render_pass_cache_ = std::make_unique<RenderPassCache>(device);
    frame_buffer_cache_ = std::make_unique<FrameBufferCache>(device);
}

int32_t FrameGraph::AddTask(const FrameGraphTaskType type, 
    const std::function<void(FrameGraphBuilder*)>& builder_func, 
    const std::function<void(FrameGraphContext*)>& exec_func) {

    //auto& task = tasks_[task_index_];

    //FrameGraphBuilder builder(texture_cache_.get(), texture_view_cache_.get(), task_index_, tasks_, resources_);
    //builder_func(&builder);

    // Resource barriers
    //for(uint32_t i = 0; i < static_cast<uint32_t>(task.write_textures_.size()); ++i) {

    /*command_buffer_->TextureMemoryBarrier(task.write_textures_[0], lgfx::MemoryState::kPresent, lgfx::MemoryState::kRenderTarget);
    command_buffer_->TextureMemoryBarrier(task.write_textures_[1], lgfx::MemoryState::kGenericRead, lgfx::MemoryState::kRenderTarget);

    task.frame_buffer_desc_.width = 800;
    task.frame_buffer_desc_.height = 600;

    lgfx::RenderPass* render_pass = render_pass_cache_->GetRenderPass(task.render_pass_desc_);
    task.frame_buffer_desc_.render_pass = render_pass;
    lgfx::FrameBuffer* frame_buffer = frame_buffer_cache_->GetFrameBuffer(task.frame_buffer_desc_);

    command_buffer_->BeginRenderPass(render_pass, frame_buffer, task.clear_desc_);

    FrameGraphContext context;
    exec_func(&context);

    command_buffer_->EndRenderPass();

    // Resource barriers
    //for(uint32_t i = 0; i < static_cast<uint32_t>(task.write_textures_.size()); ++i) {
        
    command_buffer_->TextureMemoryBarrier(task.write_textures_[0], lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kPresent);
    command_buffer_->TextureMemoryBarrier(task.write_textures_[1], lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kGenericRead);    
*/
    //++task_index_;
    return 0;
}

void FrameGraph::Execute() {

    /*command_buffer_->Close();

    device_->ExecuteCommandBuffer(lgfx::CommandBufferType::kGraphics, command_buffer_.get());

    device_->Present();

    for(uint32_t i = 0; i < task_index_; ++i) {

        tasks_[i].Clear();
    }
    

    texture_cache_->Clear();*/
    /*for(uint32_t i = 0; i < task_index_; ++i) {
        tasks_[i].clear_desc.colors.clear();
        tasks_[i].frame_buffer_desc.colors.clear();
        tasks_[i].render_pass_desc.colors.clear();
        tasks_[i].write_resources.clear();
    }
    resources_.clear();*/
    //task_index_ = 0;
}

void FrameGraph::Flush() {

    //command_buffer_->Reset();
}