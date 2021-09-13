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

    FrameGraph(Device& device) : m_device(device), m_render_pass_cache(device), m_frame_buffer_cache(device)  {
        
    }

    void create_resource(const std::string& name, const FrameGraphResourceType type, View& view, const FrameGraphResourceFlags flags) {
        m_resource_manager.create(name, type, view, flags);
    }

    void update_resource(const std::string& name, View& view) {

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

        m_tasks.clear();
        m_frame_buffer_cache.clear();
        m_render_pass_cache.clear();

        for(auto& resource : m_resource_manager.get_resources()) {
            resource.clear();
        }

        auto& outputs = m_render_passes.back().get_writes();
        assert(outputs.size() == 1 && "output resource in last pass should be 1 and has present bit flag");

        for(auto& render_pass : m_render_passes) {
           
            std::cout << format<char>("Compile pass '{}' with {} outputs, {} inputs", 
                render_pass.get_name(),
                render_pass.get_writes().size(),
                render_pass.get_reads().size()
            ) << std::endl;

            for(auto& resource : render_pass.get_reads()) {
                resource.get().release();
            }

            std::vector<std::reference_wrapper<View>> colors;

            for(auto& resource : render_pass.get_writes()) {

                colors.emplace_back(resource.get().get_view());

                resource.get().acquire();

                if(resource.get().get_after_state() != resource.get().get_before_state()) {
                    
                    m_tasks.emplace_back(ResourceTransitionTask { 
                        resource,
                        resource.get().get_before_state(), 
                        resource.get().get_after_state() 
                    });
                }
            }

            FrameGraphRenderPassCache::Key render_pass_key{};
            render_pass_key.colors = render_pass.get_attachments();
            render_pass_key.sample_count = 1;

            auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);

            FrameGraphFrameBufferCache::Key frame_buffer_key = {
                render_pass_cache,
                render_pass.get_writes()[0].get().get_view().get_resource().get_width(),
                render_pass.get_writes()[0].get().get_view().get_resource().get_height(),
                colors
            };

            m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);

            m_tasks.emplace_back(RenderPassTask { render_pass }); 
        }

        for(auto& resource : m_resource_manager.get_resources()) {
            resource.print_test();
        }

        std::cout << format<char>("{} tasks was added", m_tasks.size()) << std::endl;
    }

    void export_to_dot(const std::string& file_name) {

        std::ofstream ofs(file_name + ".dot", std::ios::beg);

        if(!ofs.is_open()) {
            
            return;
        }

        ofs << "digraph {" << std::endl;
        ofs << "rankdir=\"LR\";" << std::endl;
        ofs << "node [shape=rect, style=\"filled,rounded\", fontcolor=\"white\"];" << std::endl;

        for(auto& resource : m_resource_manager.get_resources()) {
            ofs << format<char>("Res_{} [label=\"{}\", color=\"darkslategray3\"];", resource.get_id(), resource.get_name()) << std::endl;
        }

        for(uint32 i = 0; i < m_render_passes.size(); ++i) {

            ofs << format<char>("Pass_{} [label=\"{}\", color=\"aquamarine2\"];", i, m_render_passes[i].get_name()) << std::endl;
            
            ofs << "edge [color=\"dodgerblue2\", label=\"read\", fontcolor=\"dodgerblue2\"];" << std::endl;
            for(auto& resource : m_render_passes[i].get_reads()) {
                ofs << format<char>("Res_{} -> Pass_{}", resource.get().get_id(), i) << std::endl;
            }

            ofs << "edge [color=\"firebrick3\", label=\"write\", fontcolor=\"firebrick3\"];" << std::endl;
            for(auto& resource : m_render_passes[i].get_writes()) {
                ofs << format<char>("Pass_{} -> Res_{}", i, resource.get().get_id()) << std::endl;
            }
        }

        ofs << "}" << std::endl;
    }

    void execute(CommandList& command_list) {

        command_list.reset();

        for(auto& task : m_tasks) {

            switch(task.get_type()) {

                case FrameGraphTaskType::ResourceTransition: {

                    auto& transition = std::get<ResourceTransitionTask>(task.get_task());
                    auto& resource = transition.resource.get().get_view().get_resource();
                    command_list.resource_barriers({ { resource, transition.before, transition.after } });
                    break;
                }

                case FrameGraphTaskType::RenderPass: {

                    auto render_pass = std::get<RenderPassTask>(task.get_task());

                    std::vector<std::reference_wrapper<View>> colors;
                    for(auto& resource : render_pass.render_pass.get().get_writes()) {
                        colors.emplace_back(resource.get().get_view());
                    }

                    ClearValueDesc clear_desc{};
                    for(auto& attachment : render_pass.render_pass.get().get_attachments()) {
                        clear_desc.colors.emplace_back(attachment.clear_color);
                    }

                    FrameGraphRenderPassCache::Key render_pass_key{};
                    render_pass_key.colors = render_pass.render_pass.get().get_attachments();
                    render_pass_key.sample_count = 1;

                    auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);

                    FrameGraphFrameBufferCache::Key frame_buffer_key = {
                        render_pass_cache,
                        render_pass.render_pass.get().get_writes()[0].get().get_view().get_resource().get_width(),
                        render_pass.render_pass.get().get_writes()[0].get().get_view().get_resource().get_height(),
                        colors
                    };

                    auto& frame_buffer_cache = m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);
                    
                    command_list.begin_render_pass(render_pass_cache, frame_buffer_cache, clear_desc);

                    RenderPassContext context(command_list);
                    render_pass.render_pass.get().execute(context);
                    
                    command_list.end_render_pass();
                    break;
                }
           }
        }

        command_list.close();
        m_device.get().get_command_queue(CommandListType::Graphics).execute_command_lists({ command_list });
    }

private:

    std::reference_wrapper<Device> m_device;

    FrameGraphResourceManager m_resource_manager;
    FrameGraphRenderPassCache m_render_pass_cache;
    FrameGraphFrameBufferCache m_frame_buffer_cache;

    std::vector<FrameGraphRenderPass> m_render_passes;
    std::vector<FrameGraphTask> m_tasks;
};

}