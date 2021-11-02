// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_graph.h"

using namespace ionengine::rendersystem;

lgfx::RenderPassLoadOp FrameGraphBuilder::FrameGraphResourceOpTo(const FrameGraphResourceOp op) {

    switch(op) {
        case FrameGraphResourceOp::kLoad: return lgfx::RenderPassLoadOp::kLoad;
        case FrameGraphResourceOp::kClear: return lgfx::RenderPassLoadOp::kClear;
        default: assert(false && "passed invalid argument to FrameGraphResourceWriteOpTo"); return lgfx::RenderPassLoadOp::kDontCare;
    }
}

FrameGraphBuilder::FrameGraphBuilder(
    TextureCache* texture_cache, 
    TextureViewCache* texture_view_cache, 
    lstd::object_pool<FrameGraphResource>* resource_pool, 
    lstd::object_pool<FrameGraphTask>* task_pool,
    std::vector<FrameGraphResource*>& resources,
    std::vector<FrameGraphTask*>& tasks) :
        texture_cache_(texture_cache), 
        texture_view_cache_(texture_view_cache), 
        resource_pool_(resource_pool), 
        task_pool_(task_pool),
        resources_(resources),
        tasks_(tasks) {

    task_ = tasks_.emplace_back(task_pool_->construct());

    task_->create_resources_.clear();
    task_->write_resources_.clear();
    task_->read_resources_.clear();
    task_->color_descs_.clear();
    task_->color_views_.clear();
    task_->clear_colors_.clear();
}

FrameGraphResource* FrameGraphBuilder::Create(const FrameGraphResourceDesc& desc) {

    FrameGraphResource* resource = resources_.emplace_back(resource_pool_->construct());

    if(desc.texture && desc.texture_view) {
        resource->type_ = FrameGraphResourceType::kAttachment;
        resource->texture_ = desc.texture;
        resource->texture_view_ = desc.texture_view;
        resource->presentable = desc.presentable;
    } else {
        lgfx::TextureDesc texture_desc{};
        texture_desc.dimension = lgfx::Dimension::kTexture2D;
        texture_desc.width = desc.width;
        texture_desc.height = desc.height;
        texture_desc.array_layers = desc.array_layers;
        texture_desc.mip_levels = desc.mip_levels;
        texture_desc.format = desc.format;
        texture_desc.flags = desc.flags;
        lgfx::Texture* texture = texture_cache_->GetTexture(texture_desc);

        lgfx::TextureViewDesc view_desc{};
        view_desc.dimension = texture_desc.dimension;
        view_desc.mip_level_count = texture_desc.mip_levels;
        view_desc.array_layer_count = texture_desc.array_layers;
        lgfx::TextureView* texture_view = texture_view_cache_->GetTextureView({ texture, view_desc });

        resource->type_ = FrameGraphResourceType::kAttachment;
        resource->texture_ = texture;
        resource->texture_view_ = texture_view;
    }
    task_->create_resources_.emplace_back(resource);
    return resource;

}

void FrameGraphBuilder::Read(FrameGraphResource* resource) {

}

void FrameGraphBuilder::Write(FrameGraphResource* resource, const FrameGraphResourceOp op, const Color& clear_color) {

    switch(resource->type_) {
        case FrameGraphResourceType::kAttachment: {
            task_->clear_colors_.emplace_back(lgfx::ClearValueColor { clear_color.r, clear_color.g, clear_color.b, clear_color.a });
            task_->color_descs_.emplace_back(lgfx::RenderPassColorDesc { resource->texture_->GetDesc().format, FrameGraphResourceOpTo(op), lgfx::RenderPassStoreOp::kStore });
            task_->color_views_.emplace_back(resource->texture_view_);
            break;
        }
        case FrameGraphResourceType::kBuffer: {

            break;
        }
    }
    task_->write_resources_.emplace_back(resource);
}

FrameGraph::FrameGraph(lgfx::Device* device) : 
    device_(device),
    frame_buffer_cache_(device),
    render_pass_cache_(device),
    texture_view_cache_(device),
    texture_cache_(device),
    resource_pool(kFrameGraphResourcePoolSize),
    task_pool(kFrameGraphTaskPoolSize) {

    command_buffer_ = std::make_unique<lgfx::CommandBuffer>(device, lgfx::CommandBufferType::kGraphics);
}

FrameGraphTask* FrameGraph::AddTask(const FrameGraphTaskType type, const std::function<void(FrameGraphBuilder*)>& builder_func, const std::function<void(FrameGraphContext*)>& exec_func) {

    if(tasks_.empty()) {
        command_buffer_->Reset();
    }

    FrameGraphBuilder builder(&texture_cache_, &texture_view_cache_, &resource_pool, &task_pool, resources_, tasks_);
    builder_func(&builder);

    FrameGraphTask* task = builder();

    // Resource barriers
    for(uint32_t i = 0; i < static_cast<uint32_t>(task->write_resources_.size()); ++i) {
        FrameGraphResource* resource = task->write_resources_[i];
        if(resource->texture_->GetDesc().flags & lgfx::TextureFlags::kRenderTarget) {
            if(resource->presentable) {
                command_buffer_->TextureMemoryBarrier(resource->texture_, lgfx::MemoryState::kPresent, lgfx::MemoryState::kRenderTarget);
            } else {
                command_buffer_->TextureMemoryBarrier(resource->texture_, lgfx::MemoryState::kGenericRead, lgfx::MemoryState::kRenderTarget);
            }
        }
    }

    lgfx::RenderPassDesc render_pass_desc{};
    render_pass_desc.colors = task->color_descs_;
    render_pass_desc.sample_count = 1;
    lgfx::RenderPass* render_pass = render_pass_cache_.GetRenderPass(render_pass_desc);

    lgfx::FrameBufferDesc frame_buffer_desc{};
    frame_buffer_desc.render_pass = render_pass;
    frame_buffer_desc.width = 800;
    frame_buffer_desc.height = 600;
    frame_buffer_desc.colors = task->color_views_;
    lgfx::FrameBuffer* frame_buffer = frame_buffer_cache_.GetFrameBuffer(frame_buffer_desc);

    lgfx::ClearValueDesc clear_desc{};
    clear_desc.colors = task->clear_colors_;

    command_buffer_->BeginRenderPass(render_pass, frame_buffer, clear_desc);

    FrameGraphContext context;
    exec_func(&context);

    command_buffer_->EndRenderPass();

    // Resource barriers
    for(uint32_t i = 0; i < static_cast<uint32_t>(task->write_resources_.size()); ++i) {
        FrameGraphResource* resource = task->write_resources_[i];
        if(resource->texture_->GetDesc().flags & lgfx::TextureFlags::kRenderTarget) {
            if(resource->presentable) {
                command_buffer_->TextureMemoryBarrier(resource->texture_, lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kPresent);
            } else {
                command_buffer_->TextureMemoryBarrier(resource->texture_, lgfx::MemoryState::kRenderTarget, lgfx::MemoryState::kGenericRead);
            }
        }
    }
    return task;
}

void FrameGraph::Execute() {

    command_buffer_->Close();
    device_->ExecuteCommandBuffer(lgfx::CommandBufferType::kGraphics, command_buffer_.get());
    device_->Present();

    for(uint32_t i = 0; i < static_cast<uint32_t>(tasks_.size()); ++i) {
        task_pool.destroy(tasks_[i]);
    }
    tasks_.clear();

    for(uint32_t i = 0; i < static_cast<uint32_t>(resources_.size()); ++i) {
        resource_pool.destroy(resources_[i]);
    }
    resources_.clear();

    texture_cache_.Clear();
}

void FrameGraph::Reset() {

    frame_buffer_cache_.Reset();
    render_pass_cache_.Reset();
    texture_view_cache_.Reset();
    texture_cache_.Reset();
}
