// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "context.h"
#include "resource.h"
#include "pass.h"
#include "task.h"
#include "builder.h"

namespace ionengine::renderer::fg {

class FrameGraph {
public:

    FrameGraph(api::Device& device) : m_device(device), m_render_pass_cache(device), m_frame_buffer_cache(device)  {
        
    }

    void create_resource(const std::string& name, const FrameGraphResourceType type, api::View& view, const FrameGraphResourceFlags flags) {
        m_resource_manager.create(name, type, view, flags);
    }

    void update_resource(const std::string& name, api::View& view) {

    }

    template<typename T>
    void add_task(
        const std::string& name, 
        const std::function<void(RenderPassBuilder&, T&)>& builder_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_func
    ) {
        
        T data{};

        m_added_tasks.emplace_back(RenderPassTask { name, std::bind(exec_func, std::placeholders::_1, data) });

        RenderPassBuilder builder(m_resource_manager, std::get<RenderPassTask>(m_added_tasks.back().get_task()));
        builder_func(builder, data);

        std::cout << format<char>("RenderPassTask '{}' added added to framegraph", name) << std::endl;
    }

    template<typename T>
    void add_task(
        const std::string& name, 
        const std::function<void(ComputePassBuilder&, T&)>& builder_func, 
        const std::function<void(ComputePassContext&, const T&)>& exec_func
    ) {
        
        T data{};

        m_added_tasks.emplace_back(ComputePassTask { name, std::bind(exec_func, std::placeholders::_1, data) });

        ComputePassBuilder builder(m_resource_manager, std::get<ComputePassTask>(m_added_tasks.back().get_task()));
        builder_func(builder, data);

        std::cout << format<char>("ComputePassTask '{}' added to framegraph", name) << std::endl;
    }

    void compile() {

        m_frame_buffer_cache.clear();
        m_render_pass_cache.clear();
        m_compiled_tasks.clear();

        for(auto& resource : m_resource_manager.get_resources()) {
            resource.clear();
        }

        {
            auto& writes = std::get<RenderPassTask>(m_added_tasks.back().get_task()).m_writes;
            assert(writes.size() == 1 && "output resource in last pass should be 1 and has present bit flag");
        }

        for(auto& task : m_added_tasks) {
           
            switch(task.get_type()) {

                case FrameGraphTaskType::RenderPass: {

                    auto& render_pass = std::get<RenderPassTask>(task.get_task());
                    std::cout << format<char>("Compiled task (RenderPass) '{}' ({} reads, {} writes)", 
                        render_pass.name,
                        render_pass.m_reads.size(),
                        render_pass.m_writes.size()
                    ) << std::endl;

                    for(auto& resource : render_pass.m_reads) {
                        resource.get().release();
                    }

                    for(auto& resource : render_pass.m_writes) {
                        resource.get().acquire();
                        auto states = resource.get().get_state();
                        if(states.first != states.second) {
                            m_compiled_tasks.emplace_back(ResourceTransitionTask { 
                                    resource,
                                    states.first, states.second
                                }
                            );
                        }
                    }

                    FrameGraphRenderPassCache::Key render_pass_key{};
                    render_pass_key.colors = render_pass.m_attachments;
                    render_pass_key.sample_count = 1;

                    auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);
                    auto& resource = render_pass.m_writes[0].get().get_view().get_resource();

                    std::vector<std::reference_wrapper<api::View>> colors;
                    for(auto& color : render_pass.m_writes) {
                        colors.emplace_back(color.get().get_view());
                    }

                    auto& resource_desc = std::get<api::ResourceDesc>(resource.get_desc());
                    FrameGraphFrameBufferCache::Key frame_buffer_key = {
                        render_pass_cache,
                        static_cast<uint32>(resource_desc.width),
                        resource_desc.height,
                        colors
                    };

                    auto& frame_buffer_cache = m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);
                    
                    m_compiled_tasks.emplace_back(std::get<RenderPassTask>(task.get_task()));
                    break;
                }

                case FrameGraphTaskType::ComputePass: {

                    auto& compute_pass = std::get<RenderPassTask>(task.get_task());
                    std::cout << format<char>("Compiled task (RenderPass) '{}' ({} reads, {} writes)", 
                        compute_pass.name,
                        -1, -1
                    ) << std::endl;

                    break;
                }
            }
        }

        for(auto& resource : m_resource_manager.get_resources()) {
            resource.debug_print();
        }
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

        /*for(uint32 i = 0; i < m_added_tasks.size(); ++i) {

            ofs << format<char>("Pass_{} [label=\"{}\", color=\"aquamarine2\"];", i, m_render_passes[i].get_name()) << std::endl;
            
            ofs << "edge [color=\"dodgerblue2\", label=\"read\", fontcolor=\"dodgerblue2\"];" << std::endl;
            for(auto& resource : m_render_passes[i].get_reads()) {
                ofs << format<char>("Res_{} -> Pass_{}", resource.get().get_id(), i) << std::endl;
            }

            ofs << "edge [color=\"firebrick3\", label=\"write\", fontcolor=\"firebrick3\"];" << std::endl;
            for(auto& resource : m_render_passes[i].get_writes()) {
                ofs << format<char>("Pass_{} -> Res_{}", i, resource.get().get_id()) << std::endl;
            }
        }*/

        ofs << "}" << std::endl;
    }

    void execute(api::CommandList& command_list) {

        command_list.reset();

        for(auto& task : m_compiled_tasks) {

            switch(task.get_type()) {

                case FrameGraphTaskType::ResourceTransition: {

                    auto& transition = std::get<ResourceTransitionTask>(task.get_task());
                    auto& resource = transition.resource.get().get_view().get_resource();
                    command_list.resource_barriers({ { resource, transition.before, transition.after } });
                    break;
                }

                case FrameGraphTaskType::RenderPass: {

                    auto render_pass = std::get<RenderPassTask>(task.get_task());

                    FrameGraphRenderPassCache::Key render_pass_key{};
                    render_pass_key.colors = render_pass.m_attachments;
                    render_pass_key.sample_count = 1;

                    auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);
                    auto& resource = render_pass.m_writes[0].get().get_view().get_resource();

                    std::vector<std::reference_wrapper<api::View>> colors;
                    for(auto& color : render_pass.m_writes) {
                        colors.emplace_back(color.get().get_view());
                    }

                    auto& resource_desc = std::get<api::ResourceDesc>(resource.get_desc());
                    FrameGraphFrameBufferCache::Key frame_buffer_key = {
                        render_pass_cache,
                        static_cast<uint32>(resource_desc.width),
                        resource_desc.height,
                        colors
                    };

                    auto& frame_buffer_cache = m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);
                    
                    api::ClearValueDesc clear_desc{};
                    for(auto& attachment : render_pass.m_attachments) {
                        clear_desc.colors.emplace_back(attachment.clear_color);
                    }

                    command_list.begin_render_pass(render_pass_cache, frame_buffer_cache, clear_desc);

                    RenderPassContext context(command_list);
                    render_pass.exec_func(context);
                    
                    command_list.end_render_pass();
                    break;
                }

                case FrameGraphTaskType::ComputePass: {

                    break;
                }
           }
        }

        command_list.close();
        m_device.get().get_command_queue(api::CommandListType::Graphics).execute_command_lists({ command_list });
    }

private:

    std::reference_wrapper<api::Device> m_device;

    FrameGraphResourceManager m_resource_manager;
    FrameGraphRenderPassCache m_render_pass_cache;
    FrameGraphFrameBufferCache m_frame_buffer_cache;

    std::list<FrameGraphTask> m_added_tasks;
    std::list<FrameGraphTask> m_compiled_tasks;
};

}