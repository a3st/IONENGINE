// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "context.h"
#include "pass.h"
#include "resource.h"
#include "builder.h"

namespace ionengine::renderer {

struct FrameGraphTaskDesc {
    std::reference_wrapper<FrameGraphRenderPassDesc> render_pass;
    std::vector<std::reference_wrapper<FrameGraphResourceDesc>> acquired_resources;
    std::vector<std::reference_wrapper<FrameGraphResourceDesc>> released_resources;
};

class FrameGraph {
public:

    FrameGraph(Device& device) : m_device(device)  {
        
    }

    void create_resource(const std::string& name, const AttachmentDesc& desc, View& view) {
        m_resource_manager.create(name, desc, view);
    }

    void update_resource(const std::string& name, const AttachmentDesc& desc, View& view) {

    }

    template<typename T>
    void add_pass(
        const std::string& name, 
        const std::function<void(RenderPassBuilder&, T&)>& build_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_func
    ) {
        
        T render_pass_data{};

        m_render_pass_descs.emplace_back(FrameGraphRenderPassDesc { });

        RenderPassBuilder builder(
            m_resource_manager, 
            name, 
            false, 
            std::bind(exec_func, std::placeholders::_1, render_pass_data),
            m_render_pass_descs.back()
        );

        build_func(builder, render_pass_data);

        std::cout << format<char>("RenderPass '{}' added with {} inputs, {} outputs", 
            name,
            m_render_pass_descs.back().m_input_resources.size(),
            m_render_pass_descs.back().m_output_resources.size()
        ) << std::endl;
    }

    void compile() {

       
    }

    void execute(CommandList& command_list) {

        for(auto& task_desc : m_task_descs) {

            for(auto& resource : task_desc.acquired_resources) {
                
            }

            RenderPassContext context(command_list);
            task_desc.render_pass.get().m_exec_func(context);

            for(auto& resource : task_desc.released_resources) {
                
            }
        }
    }

private:

    std::reference_wrapper<Device> m_device;

    FrameGraphResourceManager m_resource_manager;

    std::vector<FrameGraphRenderPassDesc> m_render_pass_descs;
    std::vector<FrameGraphTaskDesc> m_task_descs;
};

}