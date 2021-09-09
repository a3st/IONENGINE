// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "context.h"
#include "resource.h"
#include "pass.h"
#include "builder.h"
#include "task.h"

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
        const std::function<void(RenderPassBuilder&, T&)>& builder_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_func
    ) {
        
        T data{};

        m_render_passes.emplace_back(name, std::bind(exec_func, std::placeholders::_1, data));

        RenderPassBuilder builder(m_resource_manager, m_render_passes.back());
        builder_func(builder, data);

        std::cout << format<char>("RenderPass '{}' added", name) << std::endl;
    }

    void compile() {

        for(auto& render_pass : m_render_passes) {
           
            std::cout << format<char>("Compile pass '{}'", render_pass.get_name()) << std::endl;

            for(auto& resource : render_pass.get_inputs()) {
                resource.get().acquire();

                
            }

            for(auto& resource : render_pass.get_outputs()) {
                resource.get().release();
            }
        }
    }

    void execute(CommandList& command_list) {

        for(auto& task : m_tasks) {

           switch(task.get_type()) {

                case FrameGraphTaskType::ResourceBarrier: {

                    command_list.resource_barriers({ std::get<ResourceBarrierDesc>(task.get_task()) });
                    break;
                }

                case FrameGraphTaskType::RenderPass: {

                    RenderPassContext context(command_list);
                    std::get<std::reference_wrapper<FrameGraphRenderPass>>(task.get_task()).get().execute(context);
                    break;
                }
           }
        }
    }

private:

    std::reference_wrapper<Device> m_device;

    FrameGraphResourceManager m_resource_manager;

    std::vector<FrameGraphRenderPass> m_render_passes;
    std::vector<FrameGraphTask> m_tasks;
};

}