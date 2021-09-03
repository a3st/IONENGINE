// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "resource.h"
#include "builder.h"
#include "context.h"

namespace ionengine::renderer {

class FrameGraph {
public:

    FrameGraph(Device& device) : m_device(device)  {
        
    }

    void create_resource(const std::string& name, const std::optional<std::reference_wrapper<View>>& view) {

    }

    void update_resource(const std::string& name, const std::optional<std::reference_wrapper<View>>& view) {

    }

    template<typename T>
    void add_pass(
        const std::string& name, 
        const std::function<void(RenderPassBuilder&, T&)>& build_pass_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_pass_func
    ) {
        T pass_data{};

        RenderPassBuilder builder;
        build_pass_func(builder, pass_data);

        /*FrameGraph::RenderPassDesc render_pass_desc{};
        render_pass_desc.name = name;
        render_pass_desc.exec_func = std::bind(exec_pass_func, std::placeholders::_1, pass_data);

        m_render_passes.emplace_back(render_pass_desc);*/
    }

    void build() {

       
    }

    void execute(CommandList& command_list) {

    
    }

private:

    std::reference_wrapper<Device> m_device;

    FrameGraphResourceManager m_resource_manager;
};

}