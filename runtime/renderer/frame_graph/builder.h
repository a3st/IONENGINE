// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder(
        FrameGraphResourceManager& resource_manager,
        const std::string& name,
        const bool async,
        const std::function<void(RenderPassContext&)>& exec_func,
        FrameGraphRenderPassDesc& render_pass_desc
    ) : m_resource_manager(resource_manager), m_render_pass_desc(render_pass_desc) {
        
        m_render_pass_desc.get().m_name = name;
        m_render_pass_desc.get().m_async = async;
        m_render_pass_desc.get().m_exec_func = exec_func;
    }

    FrameGraphResourceHandle add_input(const std::string& name, const RenderPassLoadOp load_op, const RenderPassStoreOp store_op, const ClearValueColor& clear_value = { 0, 0, 0, 0 }) {
       
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);

        auto& resource_desc = m_resource_manager.get().get_resource_desc(handle);
        
        m_render_pass_desc.get().m_input_resources.emplace_back(resource_desc);
        std::cout << format<char>("got resource '{}', id = {}", resource_desc.name, resource_desc.id) << std::endl;
        return handle;
    }

    FrameGraphResourceHandle add_output(const std::string& name, const RenderPassLoadOp load_op, const RenderPassStoreOp store_op, const ClearValueColor& clear_value = { 0, 0, 0, 0 }) {
        
        FrameGraphResourceHandle handle = m_resource_manager.get().find_handle_by_name(name);

        auto& resource_desc = m_resource_manager.get().get_resource_desc(handle);
        
        m_render_pass_desc.get().m_output_resources.emplace_back(resource_desc);
        resource_desc.outputs.emplace_back(m_render_pass_desc);
        std::cout << format<char>("got resource '{}', id = {}, uses outputs passes {}", 
            resource_desc.name, resource_desc.id, resource_desc.outputs.size()) << std::endl;
        return handle;
    }

private:

    std::reference_wrapper<FrameGraphResourceManager> m_resource_manager;
    std::reference_wrapper<FrameGraphRenderPassDesc> m_render_pass_desc;
};

}