// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassBuilder {
public:

    RenderPassBuilder(FrameGraphResourceManager& resource_manager, RenderPassTask& render_pass_task) 
        : m_resource_manager(resource_manager), m_render_pass_task(render_pass_task) {
        
    }

    FrameGraphResourceHandle read(const std::string& name) {
       
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);
        auto& resource = m_resource_manager.get().get_resource(handle);
        m_render_pass_task.get().m_reads.emplace_back(resource);
        return handle;
    }

    FrameGraphResourceHandle write(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_color = { 0, 0, 0, 0 }) {
        
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);
        auto& resource = m_resource_manager.get().get_resource(handle);
        m_render_pass_task.get().m_writes.emplace_back(resource);
        m_render_pass_task.get().m_attachments.emplace_back(AttachmentDesc { resource.get_view().get_resource().get_format(), load_op, clear_color });
        return handle;
    }

private:

    std::reference_wrapper<FrameGraphResourceManager> m_resource_manager;
    std::reference_wrapper<RenderPassTask> m_render_pass_task;
};

}