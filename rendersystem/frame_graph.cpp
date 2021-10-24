// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_graph.h"

using namespace ionengine::rendersystem;

FrameGraphResource::FrameGraphResource() {

}

FrameGraphResource::FrameGraphResource(lgfx::Texture* texture, lgfx::TextureView* texture_view) :
    attachment_{ texture, texture_view }, type_(FrameGraphResourceType::Attachment) {

}

FrameGraphTask::FrameGraphTask() : 
    clear_desc_{}, render_pass_desc_{}, frame_buffer_desc_{} {

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

FrameGraphBuilder::FrameGraphBuilder(FrameGraphTask* task) : task_(task) {

}

FrameGraphResource* FrameGraphBuilder::Create(const FrameGraphResourceType type, const FrameGraphExternalResourceInfo& info) {

    FrameGraphResource resource;
    switch(type) {
        case FrameGraphResourceType::Attachment: {
            resource = FrameGraphResource(info.attachment.texture, info.attachment.texture_view);
            break;
        }
        case FrameGraphResourceType::Buffer: {
            break;
        }
    }
    auto& result = task_->resources_.emplace_back(resource);
    return &result;
}

FrameGraphResource* FrameGraphBuilder::Create(const FrameGraphResourceType type, const FrameGraphResourceInfo& info) {

    FrameGraphResource resource;
    switch(type) {
        case FrameGraphResourceType::Attachment: {
            break;
        }
        case FrameGraphResourceType::Buffer: {
            break;
        }
    }

    auto& result = task_->resources_.emplace_back(resource);
    return &result;
}

void FrameGraphBuilder::Read(FrameGraphResource* resource) {

}

void FrameGraphBuilder::Write(FrameGraphResource* resource, const FrameGraphResourceWriteOp write_op, const Color& clear_color) {

    auto to_resource_write_op = [](const FrameGraphResourceWriteOp op) -> lgfx::RenderPassLoadOp {
        switch(op) {
            case FrameGraphResourceWriteOp::Load: return lgfx::RenderPassLoadOp::kLoad;
            case FrameGraphResourceWriteOp::Clear: return lgfx::RenderPassLoadOp::kClear;
            default: assert(false && "passed invalid argument to to_resource_write_op"); return lgfx::RenderPassLoadOp::kDontCare;
        }
    };

    switch(resource->type_) {
        case FrameGraphResourceType::Attachment: {
            
            if(resource->attachment_.texture->GetFlags() & lgfx::TextureFlags::kDepthStencil) {
                task_->frame_buffer_desc_.depth_stencil = resource->attachment_.texture_view;
            } else {
                task_->frame_buffer_desc_.colors.emplace_back(resource->attachment_.texture_view);
                task_->render_pass_desc_.colors.emplace_back(lgfx::RenderPassColorDesc { resource->attachment_.texture->GetFormat(), to_resource_write_op(write_op), lgfx::RenderPassStoreOp::kStore });
                task_->clear_desc_.colors.emplace_back(lgfx::ClearValueColor { clear_color.r, clear_color.g, clear_color.b, clear_color.a });
            }

            task_->write_textures_.emplace_back(resource->attachment_.texture);
            break;
        }
        case FrameGraphResourceType::Buffer: {
            break;
        }
    }
}

FrameGraph::FrameGraph() {

}

FrameGraph::FrameGraph(lgfx::Device* device) : device_(device), task_index_(0) {

    command_buffer_ = lgfx::CommandBuffer(device, lgfx::CommandBufferType::kGraphics);
    tasks_.resize(kFrameGraphTaskSize);

    command_buffer_.Reset();
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

    FrameGraphBuilder builder(&task);
    builder_func(&builder);

    // Resource barriers
    for(uint32_t i = 0; i < static_cast<uint32_t>(task.write_textures_.size()); ++i) {
        
        command_buffer_.TextureMemoryBarrier(task.write_textures_[i], lgfx::MemoryState::kPresent, lgfx::MemoryState::kRenderTarget);
        
    }

    task.frame_buffer_desc_.width = 800;
    task.frame_buffer_desc_.height = 600;

    lgfx::RenderPass* render_pass = render_pass_cache_.GetRenderPass(task.render_pass_desc_);
    task.frame_buffer_desc_.render_pass = render_pass;
    lgfx::FrameBuffer* frame_buffer = frame_buffer_cache_.GetFrameBuffer(task.frame_buffer_desc_);
    command_buffer_.BeginRenderPass(render_pass, frame_buffer, task.clear_desc_);

    FrameGraphContext context;
    exec_func(&context);

    command_buffer_.EndRenderPass();

    // Resource barriers
    for(uint32_t i = 0; i < static_cast<uint32_t>(task.write_textures_.size()); ++i) {
        
        command_buffer_.TextureMemoryBarrier(task.write_textures_[i], lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kPresent);
        
    }

    ++task_index_;
    return &task;
}

void FrameGraph::Execute() {

    command_buffer_.Close();

    device_->ExecuteCommandBuffer(lgfx::CommandBufferType::kGraphics, &command_buffer_);

    device_->Present();

    for(uint32_t i = 0; i < task_index_; ++i) {

        tasks_[i].Clear();
    }
    task_index_ = 0;
}

void FrameGraph::Flush() {

    command_buffer_.Reset();
}