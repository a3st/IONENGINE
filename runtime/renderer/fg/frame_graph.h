// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "types.h"
#include "context.h"
#include "task.h"
#include "resource.h"
#include "../pass_cache.h"
#include "builder.h"
#include "timeline.h"

namespace ionengine::renderer::fg {

class FrameGraph {
public:

    FrameGraph(api::Device& device) : m_device(device), m_render_pass_cache(device), m_frame_buffer_cache(device)  {
        
    }

    void create_resource(const std::string& name, const ResourceType type, api::View& view, const ResourceFlags flags) {
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

        m_tasks.emplace_back(TaskType::RenderPass, name, RenderPassTask { std::bind(exec_func, std::placeholders::_1, data) });

        RenderPassBuilder builder(m_resource_manager, m_tasks.back());
        builder_func(builder, data);

        std::cout << format<char>("RenderPass '{}' added added to framegraph", name) << std::endl;
    }

    template<typename T>
    void add_task(
        const std::string& name, 
        const std::function<void(ComputePassBuilder&, T&)>& builder_func, 
        const std::function<void(ComputePassContext&, const T&)>& exec_func
    ) {
        
        T data{};

        m_tasks.emplace_back(TaskType::ComputePass, name, ComputePassTask { std::bind(exec_func, std::placeholders::_1, data) });

        ComputePassBuilder builder(m_resource_manager, m_tasks.back());
        builder_func(builder, data);

        std::cout << format<char>("ComputePass '{}' added to framegraph", name) << std::endl;
    }

    void compile() {

        m_frame_buffer_cache.clear();
        m_render_pass_cache.clear();

        for(auto& task : m_tasks) {
            task.m_ref_count = static_cast<uint32>(task.m_writes.size());
        }

        for(auto& resource : m_resource_manager.m_resources) {
            resource.m_ref_count = static_cast<uint32>(resource.m_readers.size());
        }

        std::stack<std::reference_wrapper<Resource>> unref_resources;
        for(auto& resource : m_resource_manager.m_resources) {
            if(resource.m_ref_count == 0 && resource.culling()) {
                unref_resources.emplace(resource);
            }
        }

        while(!unref_resources.empty()) {
            auto unref_resource = unref_resources.top();
            unref_resources.pop();

            for(auto writer : unref_resource.get().m_writers) {
                if(writer.get().m_ref_count > 0) {
                    writer.get().m_ref_count--;
                }
                if(writer.get().m_ref_count == 0) {
                    for(auto read : writer.get().m_reads) {
                        auto read_resource = m_resource_manager.get_by_handle(read);
                        if(read_resource.value().get().m_ref_count > 0) {
                            read_resource.value().get().m_ref_count--;
                        }
                        if(read_resource.value().get().m_ref_count == 0) {
                            unref_resources.emplace(read_resource.value());
                        }
                    }
                }
            }
        }

        m_timelines.clear();

        for(auto& task : m_tasks) {

            if(task.m_ref_count == 0) {
                std::cout << "Warning: " << task.get_name() << " is culling" << std::endl;
                continue;
            }
           
            switch(task.get_type()) {

                case TaskType::RenderPass: {

                    std::vector<std::reference_wrapper<Resource>> m_invalidates;
                    std::vector<std::reference_wrapper<Resource>> m_flushes;

                    for(auto& handle : task.m_writes) {

                        auto& resource = m_resource_manager.get_by_handle(handle);
                        m_flushes.emplace_back(resource);
                        //auto result = resource.value().get().barrier();
                    }

                    for(auto& handle : task.m_reads) {

                        auto& resource = m_resource_manager.get_by_handle(handle);
                        m_invalidates.emplace_back(resource);
                    }

                    for(auto& resource : m_flushes) {

                        
                    }

                    /*auto& render_pass = std::get<RenderPassTask>(task.get_task());
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

                    RenderPassCache::Key render_pass_key{};
                    render_pass_key.colors = render_pass.m_color_descs;
                    render_pass_key.sample_count = 1;

                    auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);
                    auto& resource = render_pass.m_writes[0].get().get_view().get_resource();

                    std::vector<std::reference_wrapper<api::View>> colors;
                    for(auto& color : render_pass.m_writes) {
                        colors.emplace_back(color.get().get_view());
                    }

                    auto& resource_desc = std::get<api::ResourceDesc>(resource.get_desc());
                    FrameBufferCache::Key frame_buffer_key = {
                        render_pass_cache,
                        static_cast<uint32>(resource_desc.width),
                        resource_desc.height,
                        colors
                    };

                    auto& frame_buffer_cache = m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);
                    
                    m_compiled_tasks.emplace_back(std::get<RenderPassTask>(task.get_task()));*/
                    break;
                }

                case TaskType::ComputePass: {

                    /*auto& compute_pass = std::get<RenderPassTask>(task.get_task());
                    std::cout << format<char>("Compiled task (RenderPass) '{}' ({} reads, {} writes)", 
                        compute_pass.name,
                        -1, -1
                    ) << std::endl;*/

                    break;
                }
            }
        }

        for(auto& resource : m_resource_manager.m_resources) {
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

        for(auto& resource : m_resource_manager.m_resources) {
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

        /*for(auto& task : m_compiled_tasks) {

            switch(task.get_type()) {

                case TaskType::ResourceTransition: {
                    auto& resource_transition = std::get<ResourceTransitionTask>(task.get_task());
                    auto& resource = resource_transition.resource.get().get_view().get_resource();
                    command_list.resource_barriers({ { resource, resource_transition.before, resource_transition.after } });
                    break;
                }

                case TaskType::RenderPass: {
                    auto render_pass = std::get<RenderPassTask>(task.get_task());

                    RenderPassCache::Key render_pass_key{};
                    render_pass_key.colors = render_pass.m_color_descs;
                    render_pass_key.sample_count = 1;

                    auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);
                    auto& resource = render_pass.m_writes[0].get().get_view().get_resource();

                    std::vector<std::reference_wrapper<api::View>> colors;
                    for(auto& color : render_pass.m_writes) {
                        colors.emplace_back(color.get().get_view());
                    }

                    auto& resource_desc = std::get<api::ResourceDesc>(resource.get_desc());
                    FrameBufferCache::Key frame_buffer_key = {
                        render_pass_cache,
                        static_cast<uint32>(resource_desc.width),
                        resource_desc.height,
                        colors
                    };

                    auto& frame_buffer_cache = m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);
                    
                    api::ClearValueDesc clear_desc{};
                    clear_desc.colors = render_pass.m_colors_clear_values;

                    command_list.begin_render_pass(render_pass_cache, frame_buffer_cache, clear_desc);

                    RenderPassContext context(command_list);
                    render_pass.exec_func(context);
                    
                    command_list.end_render_pass();
                    break;
                }

                case TaskType::ComputePass: {
                    auto compute_pass = std::get<ComputePassTask>(task.get_task());

                    break;
                }
           }
        }*/

        command_list.close();
        m_device.get().get_command_queue(api::CommandListType::Graphics).execute_command_lists({ command_list });
    }

private:

    std::reference_wrapper<api::Device> m_device;

    ResourceManager m_resource_manager;
    RenderPassCache m_render_pass_cache;
    FrameBufferCache m_frame_buffer_cache;

    std::list<Task> m_tasks;
    std::list<Timeline> m_timelines;
};

}