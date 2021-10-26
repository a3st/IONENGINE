// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_graph.h"

using namespace ionengine::rendersystem;

/*lgfx::RenderPassLoadOp FrameGraphBuilder::FrameGraphResourceWriteOpTo(const FrameGraphResourceWriteOp op) {

    switch(op) {
        case FrameGraphResourceWriteOp::kLoad: return lgfx::RenderPassLoadOp::kLoad;
        case FrameGraphResourceWriteOp::kClear: return lgfx::RenderPassLoadOp::kClear;
        default: assert(false && "passed invalid argument to FrameGraphResourceWriteOpTo"); return lgfx::RenderPassLoadOp::kDontCare;
    }
}*/


FrameGraphBuilder::FrameGraphBuilder(
    TextureCache* texture_cache, 
    TextureViewCache* texture_view_cache, 
    lstd::object_pool<FrameGraphResource>* resource_pool, 
    lstd::object_pool<FrameGraphTask>* task_pool,
    std::vector<lstd::unique_object_ptr<FrameGraphResource>>& resources,
    std::vector<lstd::unique_object_ptr<FrameGraphTask>>& tasks) :
        texture_cache_(texture_cache), 
        texture_view_cache_(texture_view_cache), 
        resource_pool_(resource_pool), 
        task_pool_(task_pool),
        resources_(resources),
        tasks_(tasks) {

    auto& result = tasks_.emplace_back(lstd::make_unique_object_ptr(task_pool_));
    task_ = result.get();
}

FrameGraphResource* FrameGraphBuilder::Create(const FrameGraphResourceDesc& desc) {

    auto& result = resources_.emplace_back(lstd::make_unique_object_ptr(resource_pool_));
    FrameGraphResource* resource = result.get();

    if(desc.texture && desc.texture_view) {
        resource->type_ = FrameGraphResourceType::kAttachment;
        resource->texture_ = desc.texture;
        resource->texture_view_ = desc.texture_view;
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
    return resource;

}

void FrameGraphBuilder::Read(FrameGraphResource* resource) {

}

void FrameGraphBuilder::Write(FrameGraphResource* resource, const FrameGraphResourceOp op, const Color& clear_color) {

    switch(resource->type_) {
        case FrameGraphResourceType::kAttachment: {
            
            break;
        }
    }
}

FrameGraph::FrameGraph(lgfx::Device* device) : 
    device_(device),
    frame_buffer_cache_(device),
    render_pass_cache_(device),
    texture_view_cache_(device),
    texture_cache_(device),
    resource_pool(kFrameGraphResourcePoolSize),
    task_pool(kFrameGraphTaskPoolSize) {

    //command_buffer_ = std::make_unique<lgfx::CommandBuffer>(device, lgfx::CommandBufferType::kGraphics);
    //command_buffer_->Reset();
}

FrameGraphTask* FrameGraph::AddTask(const FrameGraphTaskType type, const std::function<void(FrameGraphBuilder*)>& builder_func, const std::function<void(FrameGraphContext*)>& exec_func) {

    FrameGraphBuilder builder(&texture_cache_, &texture_view_cache_, &resource_pool, &task_pool, resources_, tasks_);
    builder_func(&builder);

    //auto& task = tasks_[task_index_];

    //
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

    return builder();
}

void FrameGraph::Execute() {

    /*command_buffer_->Close();

    device_->ExecuteCommandBuffer(lgfx::CommandBufferType::kGraphics, command_buffer_.get());

    device_->Present();
    */

    tasks_.clear();
    resources_.clear();

    texture_cache_.Clear();
}

void FrameGraph::Flush() {

    //command_buffer_->Reset();
}

void FrameGraph::Reset() {

    frame_buffer_cache_.Reset();
    render_pass_cache_.Reset();
    texture_view_cache_.Reset();
    texture_cache_.Reset();
}