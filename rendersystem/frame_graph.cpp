// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_graph.h"

using namespace ionengine::rendersystem;

FrameGraphResource::FrameGraphResource() {

}

FrameGraphResource::FrameGraphResource(lgfx::Texture* texture, lgfx::TextureView* texture_view) :
    attachment_{ texture, texture_view }, type_(FrameGraphResourceType::kAttachment) {

}

FrameGraphTask::FrameGraphTask() : 
    clear_desc_{}, render_pass_desc_{}, frame_buffer_desc_{} {
    
    resources_.reserve(10);
}

void FrameGraphTask::Clear() {

    resources_.clear();
    write_textures_.clear();
    frame_buffer_desc_.colors.clear();
    render_pass_desc_.colors.clear();
    clear_desc_.colors.clear();
    frame_buffer_desc_.depth_stencil = nullptr;
}

FrameGraphContext::FrameGraphContext() {
    
}

FrameGraphBuilder::FrameGraphBuilder(TextureCache* texture_cache, TextureViewCache* texture_view_cache, FrameGraphTask* task) : 
    texture_cache_(texture_cache), texture_view_cache_(texture_view_cache), task_(task) {

}

FrameGraphResourceId FrameGraphBuilder::Create(const FrameGraphResourceType type, const FrameGraphExternalResourceInfo& info) {

    switch(type) {
        case FrameGraphResourceType::kAttachment: {
            task_->resources_.emplace_back(info.attachment.texture, info.attachment.texture_view);
            return FrameGraphResourceId(task_->resources_.size() - 1);
        }
        case FrameGraphResourceType::kBuffer: {
            return FrameGraphResourceId(0);
        }
        default: return FrameGraphResourceId(0);
    }
}

FrameGraphResourceId FrameGraphBuilder::Create(const FrameGraphResourceType type, const FrameGraphResourceInfo& info) {

    switch(type) {
        case FrameGraphResourceType::kAttachment: {
            lgfx::TextureDesc desc{};
            desc.dimension = lgfx::Dimension::kTexture2D;
            desc.width = info.attachment.width;
            desc.height = info.attachment.height;
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

            task_->resources_.emplace_back(texture, texture_view);
            return FrameGraphResourceId(task_->resources_.size() - 1);
        }
        case FrameGraphResourceType::kBuffer: {
            return FrameGraphResourceId(0);
        }
        default: return FrameGraphResourceId(0);
    }
}

void FrameGraphBuilder::Read(const FrameGraphResourceId& resource_id) {

}

void FrameGraphBuilder::Write(const FrameGraphResourceId& resource_id, const FrameGraphResourceWriteOp write_op, const Color& clear_color) {

    auto to_resource_write_op = [](const FrameGraphResourceWriteOp op) -> lgfx::RenderPassLoadOp {
        switch(op) {
            case FrameGraphResourceWriteOp::kLoad: return lgfx::RenderPassLoadOp::kLoad;
            case FrameGraphResourceWriteOp::kClear: return lgfx::RenderPassLoadOp::kClear;
            default: assert(false && "passed invalid argument to to_resource_write_op"); return lgfx::RenderPassLoadOp::kDontCare;
        }
    };

    auto& resource = task_->resources_[resource_id.id_];

    std::cout << (bool)resource.attachment_.texture_view << std::endl;

    switch(resource.type_) {
        case FrameGraphResourceType::kAttachment: {
            
            if(resource.attachment_.texture->GetDesc().flags & lgfx::TextureFlags::kDepthStencil) {
                task_->frame_buffer_desc_.depth_stencil = resource.attachment_.texture_view;
            } else {
                task_->frame_buffer_desc_.colors.emplace_back(resource.attachment_.texture_view);
                task_->render_pass_desc_.colors.emplace_back(lgfx::RenderPassColorDesc { resource.attachment_.texture->GetDesc().format, to_resource_write_op(write_op), lgfx::RenderPassStoreOp::kStore });
                task_->clear_desc_.colors.emplace_back(lgfx::ClearValueColor { clear_color.r, clear_color.g, clear_color.b, clear_color.a });
            }
            task_->write_textures_.emplace_back(resource.attachment_.texture);
            break;
        }
        case FrameGraphResourceType::kBuffer: {
            break;
        }
    }
}

FrameGraph::FrameGraph() {

}

FrameGraph::FrameGraph(lgfx::Device* device) : device_(device), task_index_(0) {

    command_buffer_ = std::make_unique<lgfx::CommandBuffer>(device, lgfx::CommandBufferType::kGraphics);
    tasks_.resize(kFrameGraphTaskSize);

    command_buffer_->Reset();

    texture_cache_ = TextureCache(device);
    texture_view_cache_ = TextureViewCache(device);
}

FrameGraph::FrameGraph(FrameGraph&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(frame_buffer_cache_, rhs.frame_buffer_cache_);
    std::swap(render_pass_cache_, rhs.render_pass_cache_);
    std::swap(texture_view_cache_, rhs.texture_view_cache_);
    std::swap(texture_cache_, rhs.texture_cache_);
    std::swap(command_buffer_, rhs.command_buffer_);
    std::swap(tasks_, rhs.tasks_);
    std::swap(task_index_, rhs.task_index_);
}

FrameGraph& FrameGraph::operator=(FrameGraph&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(frame_buffer_cache_, rhs.frame_buffer_cache_);
    std::swap(render_pass_cache_, rhs.render_pass_cache_);
    std::swap(texture_view_cache_, rhs.texture_view_cache_);
    std::swap(texture_cache_, rhs.texture_cache_);
    std::swap(command_buffer_, rhs.command_buffer_);
    std::swap(tasks_, rhs.tasks_);
    std::swap(task_index_, rhs.task_index_);
    return *this;
}

FrameGraphTask* FrameGraph::AddTask(const FrameGraphTaskType type, 
    const std::function<void(FrameGraphBuilder*)>& builder_func, 
    const std::function<void(FrameGraphContext*)>& exec_func) {

    auto& task = tasks_[task_index_];

    FrameGraphBuilder builder(&texture_cache_, &texture_view_cache_, &task);
    builder_func(&builder);

    // Resource barriers
    //for(uint32_t i = 0; i < static_cast<uint32_t>(task.write_textures_.size()); ++i) {

    command_buffer_->TextureMemoryBarrier(task.write_textures_[0], lgfx::MemoryState::kPresent, lgfx::MemoryState::kRenderTarget);
    command_buffer_->TextureMemoryBarrier(task.write_textures_[1], lgfx::MemoryState::kGenericRead, lgfx::MemoryState::kRenderTarget);

    task.frame_buffer_desc_.width = 800;
    task.frame_buffer_desc_.height = 600;

    lgfx::RenderPass* render_pass = render_pass_cache_.GetRenderPass(task.render_pass_desc_);
    task.frame_buffer_desc_.render_pass = render_pass;
    lgfx::FrameBuffer* frame_buffer = frame_buffer_cache_.GetFrameBuffer(task.frame_buffer_desc_);

    command_buffer_->BeginRenderPass(render_pass, frame_buffer, task.clear_desc_);

    FrameGraphContext context;
    exec_func(&context);

    command_buffer_->EndRenderPass();

    // Resource barriers
    //for(uint32_t i = 0; i < static_cast<uint32_t>(task.write_textures_.size()); ++i) {
        
    command_buffer_->TextureMemoryBarrier(task.write_textures_[0], lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kPresent);
    command_buffer_->TextureMemoryBarrier(task.write_textures_[1], lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kGenericRead);    

    ++task_index_;
    return &task;
}

void FrameGraph::Execute() {

    command_buffer_->Close();

    device_->ExecuteCommandBuffer(lgfx::CommandBufferType::kGraphics, command_buffer_.get());

    device_->Present();

    for(uint32_t i = 0; i < task_index_; ++i) {

        tasks_[i].Clear();
    }
    task_index_ = 0;

    texture_cache_.Clear();
}

void FrameGraph::Flush() {

    command_buffer_->Reset();
}