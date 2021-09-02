// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "resource.h"
#include "builder.h"
#include "context.h"

namespace ionengine::renderer {

class FrameGraph {
public:

    struct RenderPassDesc {
        std::string name;
        
        std::vector<FrameGraphResource> input_resources;
        std::vector<FrameGraphResource> output_resources;

        std::function<void(RenderPassContext&)> exec_func;
    };

    struct ComputePassDesc {
        std::string name;
    };

    enum class TaskType {
        RenderPass,
        ComputePass,
        Present
    };

    struct TaskDesc {
        TaskType type;
        uint32 index;
    };

    FrameGraph(Device& device) : m_device(device)  {
        
    }

    void bind_resource(const std::string& name, View& view) {

        m_bind_resources.emplace(name, view);
    }

    template<enum class FrameGraphResourceType T>
    View& get_resource(const FrameGraphResource& resource) { 
        return m_bind_resources.at("123");
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

        FrameGraph::RenderPassDesc render_pass_desc{};
        render_pass_desc.name = name;
        render_pass_desc.input_resources = builder.get_input_resources();
        render_pass_desc.output_resources = builder.get_output_resources();
        render_pass_desc.exec_func = std::bind(exec_pass_func, std::placeholders::_1, pass_data);

        m_render_passes.emplace_back(render_pass_desc);
    }

    void build() {

        for(auto& render_pass : m_render_passes) {

            std::cout << render_pass.name << std::endl;
        }
    }

    void execute(CommandList& command_list) {

        for(auto& task : m_tasks) {

            switch(task.type) {

                case FrameGraph::TaskType::RenderPass: {

                    auto& render_pass_desc = m_render_passes[task.index];


                    RenderPassContext render_pass_context(command_list);
                    
                    

                    render_pass_desc.exec_func(render_pass_context);
                    break;
                }

                case FrameGraph::TaskType::ComputePass: {

                    break;
                }
            }
        }

        m_tasks.clear();
        m_render_passes.clear();
        m_compute_passes.clear();
        m_bind_resources.clear();
    }

private:

    std::reference_wrapper<Device> m_device;

    std::vector<FrameGraph::TaskDesc> m_tasks;

    std::vector<FrameGraph::RenderPassDesc> m_render_passes;
    std::vector<FrameGraph::ComputePassDesc> m_compute_passes;
    std::map<std::string, std::reference_wrapper<View>> m_bind_resources;
};

}