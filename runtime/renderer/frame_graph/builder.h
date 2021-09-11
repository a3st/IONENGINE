// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder(FrameGraphResourceManager& resource_manager, FrameGraphRenderPass& render_pass) 
        : m_resource_manager(resource_manager), m_render_pass(render_pass) {
        
    }

    FrameGraphResourceHandle read(const std::string& name) {
       
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);
        auto& resource = m_resource_manager.get().get_resource(handle);
        
        m_render_pass.get().add_read(resource);
        return handle;
    }

    FrameGraphResourceHandle write(const std::string& name, const RenderPassLoadOp load_op, const RenderPassStoreOp store_op, const ClearValueColor& clear_color = { 0, 0, 0, 0 }) {
        
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);
        auto& resource = m_resource_manager.get().get_resource(handle);

        m_render_pass.get().add_write(resource, load_op, store_op, clear_color);
        return handle;
    }

private:

    std::reference_wrapper<FrameGraphResourceManager> m_resource_manager;
    std::reference_wrapper<FrameGraphRenderPass> m_render_pass;
};

}