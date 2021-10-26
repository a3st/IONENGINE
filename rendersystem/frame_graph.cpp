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

FrameGraphContext::FrameGraphContext() {
    
}

FrameGraphBuilder::FrameGraphBuilder(TextureCache* texture_cache, TextureViewCache* texture_view_cache, std::vector<FrameGraphResource>* resources, std::vector<FrameGraphTask>* tasks) :
    texture_cache_(texture_cache), texture_view_cache_(texture_view_cache), resources_(resources), tasks_(tasks) {

    tasks_->emplace_back(FrameGraphTaskType::kRenderPass);
    task_id_ = FrameGraphTaskId(static_cast<uint32_t>(tasks_->size()) - 1);
}

FrameGraphResourceId FrameGraphBuilder::Create(const FrameGraphResourceDesc& desc) {

    auto& task = tasks_->at(task_id_.id_);

    if(desc.texture && desc.texture_view) {
        resources_->emplace_back(desc.type, desc.texture, desc.texture_view);
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

        resources_->emplace_back(desc.type, texture, texture_view);
    }
    return FrameGraphResourceId(static_cast<uint32_t>(resources_->size()) - 1);

}

//void FrameGraphBuilder::Read(const FrameGraphResourceId& resource_id) {
//}

void FrameGraphBuilder::Write(const FrameGraphResourceId& resource_id, const FrameGraphResourceOp op, const Color& clear_color) {

    auto& task = tasks_->at(task_id_.id_);
    auto& resource = resources_->at(resource_id.id_);

    switch(resource.GetType()) {
        case FrameGraphResourceType::kAttachment: {
            
            break;
        }
    }
}

FrameGraph::FrameGraph(lgfx::Device* device) : device_(device) {

    //command_buffer_ = std::make_unique<lgfx::CommandBuffer>(device, lgfx::CommandBufferType::kGraphics);
    //command_buffer_->Reset();

    texture_cache_ = std::make_unique<TextureCache>(device);
    texture_view_cache_ = std::make_unique<TextureViewCache>(device);
    render_pass_cache_ = std::make_unique<RenderPassCache>(device);
    frame_buffer_cache_ = std::make_unique<FrameBufferCache>(device);

    // Object Pool Test
    color_descs = lstd::object_pool<lgfx::RenderPassColorDesc>(10);
    for(uint32_t i = 0; i < 20; ++i) {
        auto obj = lstd::make_unique_object_ptr<lgfx::RenderPassColorDesc>(&color_descs);
        obj->format = lgfx::Format::kRG8unorm;
        //std::cout << "set format: " << (int32_t)obj->format << std::endl;
    }
}

FrameGraphTaskId FrameGraph::AddTask(const FrameGraphTaskType type, 
    const std::function<void(FrameGraphBuilder*)>& builder_func, 
    const std::function<void(FrameGraphContext*)>& exec_func) {

    FrameGraphBuilder builder(texture_cache_.get(), texture_view_cache_.get(), &resources_, &tasks_);
    builder_func(&builder);

    FrameGraphTaskId task_id = builder.GetResult();

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
    //++task_index_;
    return task_id;
}

void FrameGraph::Execute() {

    /*command_buffer_->Close();

    device_->ExecuteCommandBuffer(lgfx::CommandBufferType::kGraphics, command_buffer_.get());

    device_->Present();

    for(uint32_t i = 0; i < task_index_; ++i) {

        tasks_[i].Clear();
    }
    

    */
    /*for(uint32_t i = 0; i < task_index_; ++i) {
        tasks_[i].clear_desc.colors.clear();
        tasks_[i].frame_buffer_desc.colors.clear();
        tasks_[i].render_pass_desc.colors.clear();
        tasks_[i].write_resources.clear();
    }
    */
    //task_index_ = 0;

    tasks_.clear();
    resources_.clear();
    texture_cache_->Clear();
}

void FrameGraph::Flush() {

    //command_buffer_->Reset();
}