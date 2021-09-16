// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassBuilder {
public:

    RenderPassBuilder(FrameGraphResourceManager& resource_manager, RenderPassTask& task) 
        : m_resource_manager(resource_manager), m_task(task) {
        
    }

    FrameGraphResourceHandle read(const std::string& name) {
       
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);
        auto& resource = m_resource_manager.get().get_resource(handle);
        m_task.get().m_reads.emplace_back(resource);
        return handle;
    }

    FrameGraphResourceHandle write(const std::string& name, const api::RenderPassLoadOp load_op, const api::ClearValueColor& clear_color = { 0, 0, 0, 0 }) {
        
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);
        auto& resource = m_resource_manager.get().get_resource(handle);
        m_task.get().m_writes.emplace_back(resource);
        auto resource_desc = std::get<api::ResourceDesc>(resource.get_view().get_resource().get_desc());
        m_task.get().m_attachments.emplace_back(AttachmentDesc { resource_desc.format, load_op, clear_color });
        return handle;
    }

private:

    std::reference_wrapper<FrameGraphResourceManager> m_resource_manager;
    std::reference_wrapper<RenderPassTask> m_task;
};

class ComputePassBuilder {
public:

    ComputePassBuilder(FrameGraphResourceManager& resource_manager, ComputePassTask& task) 
        : m_resource_manager(resource_manager), m_task(task) {
        
    }

    FrameGraphResourceHandle read(const std::string& name) {
        return FrameGraphResourceHandle::null();
    }

    FrameGraphResourceHandle write(const std::string& name) {
        return FrameGraphResourceHandle::null();
    }

private:

    std::reference_wrapper<FrameGraphResourceManager> m_resource_manager;
    std::reference_wrapper<ComputePassTask> m_task;
};

}