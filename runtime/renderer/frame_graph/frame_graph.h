// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "context.h"
#include "pass.h"
#include "resource.h"
#include "builder.h"

namespace ionengine::renderer {

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
        const std::function<void(RenderPassBuilder&, T&)>& build_pass_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_pass_func
    ) {
        T pass_data{};

        RenderPassBuilder builder(m_resource_manager);
        build_pass_func(builder, pass_data);

        //FrameGraphRenderPass render_pass;
        //render_pass_desc.name = name;
        //render_pass_desc.exec_func = std::bind(exec_pass_func, std::placeholders::_1, pass_data);

        //m_render_passes.emplace_back(render_pass_desc);
    }

    void compile() {

       
    }

    void execute(CommandList& command_list) {

    
    }

private:

    std::reference_wrapper<Device> m_device;

    FrameGraphResourceManager m_resource_manager;
};

}