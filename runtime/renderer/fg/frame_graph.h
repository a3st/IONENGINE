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
                    auto& render_pass_task = std::get<RenderPassTask>(task.m_task);

                    std::vector<api::ResourceBarrierDesc> barriers;
                    for(auto& handle : task.m_writes) {
                        auto& resource = m_resource_manager.get_by_handle(handle).value();
                        auto result = add_transition_barrier(BarrierType::Write, resource, task);
                        if(result.has_value()) {
                            barriers.emplace_back(result.value());
                        }
                    }
                    for(auto& handle : task.m_reads) {
                        auto& resource = m_resource_manager.get_by_handle(handle).value();
                        auto result = add_transition_barrier(BarrierType::Read, resource, task);
                        if(result.has_value()) {
                            barriers.emplace_back(result.value());
                        }
                    }
                    m_timelines.emplace_back(TimelineType::Barrier, BarrierTimeline { barriers });

                    std::vector<api::RenderPassColorDesc> color_descs;
                    api::RenderPassDepthStencilDesc depth_stencil_desc;
                    std::vector<std::reference_wrapper<api::View>> color_views;
                    std::optional<std::reference_wrapper<api::View>> depth_stencil_view;
                    api::ClearValueDesc clear_value_desc;
                    for(uint32 i = 0; i < task.m_writes.size(); ++i) {
                        auto& resource = m_resource_manager.get_by_handle(task.m_writes[i]).value();
                        auto& view = resource.get().get_view();
                        auto& resource_desc = std::get<api::ResourceDesc>(view.get_resource().get_desc());

                        auto& clear_color = render_pass_task.attachments[i].clear_color;
                        clear_value_desc.colors.emplace_back(api::ClearValueColor { clear_color.r, clear_color.g, clear_color.b, clear_color.a });

                        api::RenderPassLoadOp load_op;
                        switch(render_pass_task.attachments[i].op) {
                            case ResourceOp::Clear: load_op = api::RenderPassLoadOp::Clear; break;
                            case ResourceOp::Load: load_op = api::RenderPassLoadOp::Load; break;
                        }
                        
                        if(resource.get().get_flags() & ResourceFlags::DepthStencil) {
                            depth_stencil_desc = api::RenderPassDepthStencilDesc {
                                resource_desc.format,
                                load_op,
                                api::RenderPassStoreOp::Store,
                                load_op,
                                api::RenderPassStoreOp::Store
                            };
                            depth_stencil_view = resource.get().get_view();
                        } else {
                            color_descs.emplace_back(
                                api::RenderPassColorDesc { 
                                    resource_desc.format,
                                    load_op,
                                    api::RenderPassStoreOp::Store
                                }
                            );
                            color_views.emplace_back(resource.get().get_view());
                        }
                    }

                    m_timelines.emplace_back(
                        TimelineType::RenderPass, RenderPassTimeline { 
                            color_descs, 
                            depth_stencil_desc, 
                            1, 
                            color_views, 
                            depth_stencil_view,
                            clear_value_desc,
                            render_pass_task.exec_func
                        }
                    );

                    barriers.clear();
                    for(auto& handle : task.m_writes) {
                        auto& resource = m_resource_manager.get_by_handle(handle).value();
                        if(resource.get().get_flags() & ResourceFlags::Present) {
                            auto result = add_transition_barrier(BarrierType::Present, resource, task);
                            if(result.has_value()) {
                                barriers.emplace_back(result.value());
                            }
                        }
                    }
                    m_timelines.emplace_back(TimelineType::Barrier, BarrierTimeline { barriers });

                    std::cout << format<char>("RenderPass '{}' is compiled", task.get_name()) << std::endl;
                    break;
                }

                case TaskType::ComputePass: {
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

        for(auto& timeline : m_timelines) {

            switch(timeline.get_type()) {

                case TimelineType::Barrier: {
                    auto& barrier = std::get<BarrierTimeline>(timeline.get_timeline());
                    command_list.resource_barriers(barrier.data);
                    break;
                }
                case TimelineType::RenderPass: {
                    auto render_pass = std::get<RenderPassTimeline>(timeline.get_timeline());

                    RenderPassCache::Key render_pass_key = {
                        render_pass.color_descs,
                        render_pass.depth_stencil_desc,
                        render_pass.sample_count
                    };
                    auto& render_pass_cache = m_render_pass_cache.get_render_pass(render_pass_key);
                    
                    // change width height todo
                    FrameBufferCache::Key frame_buffer_key = {
                        render_pass_cache,
                        800,
                        600,
                        render_pass.color_views,
                        render_pass.depth_stencil_view
                    };
                    auto& frame_buffer_cache = m_frame_buffer_cache.get_frame_buffer(frame_buffer_key);
                    
                    command_list.begin_render_pass(render_pass_cache, frame_buffer_cache, render_pass.clear_value_desc);
                    RenderPassContext context(command_list);
                    render_pass.exec_func(context);
                    command_list.end_render_pass();
                    break;
                }

                case TimelineType::ComputePass: {
                    break;
                }
           }
        }

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

    std::optional<api::ResourceBarrierDesc> add_transition_barrier(const BarrierType type, Resource& resource, Task& task) {
        
        auto& task_it = std::find_if(
            m_tasks.begin(), 
            m_tasks.end(), 
            [&](Task& element) { 
                return element.get_name() == task.get_name(); 
            }
        );

        api::ResourceState src_state;
        for(auto& handle : task_it->m_reads) {
            if(ResourceHandle(resource.m_id) == handle) {
                src_state = api::ResourceState::PixelShaderResource;
                break;
            }
        }
        for(auto& handle : task_it->m_writes) {
            if(ResourceHandle(resource.m_id) == handle) {
                src_state = api::ResourceState::RenderTarget;
                break;
            }
        }

        api::ResourceState dst_state;
        switch(resource.get_type()) {

            case ResourceType::Attachment: {
                switch(type) {
                    case BarrierType::Read: {
                        dst_state = api::ResourceState::PixelShaderResource;
                    }
                    case BarrierType::Write: {
                        dst_state = api::ResourceState::RenderTarget;
                    }
                    case BarrierType::Present: {
                        dst_state = api::ResourceState::Present;
                    }
                }
            }
            case ResourceType::Buffer: {
                break;
            }
        }

        api::ResourceState last_state = api::ResourceState::Common;
        for(auto& it = m_tasks.begin(); it != task_it; ++it) {
            for(auto& handle : it->m_reads) {
                if(ResourceHandle(resource.m_id) == handle) {
                    last_state = api::ResourceState::PixelShaderResource;
                    break;
                }
            }
            for(auto& handle : it->m_writes) {
                if(ResourceHandle(resource.m_id) == handle) {
                    last_state = api::ResourceState::RenderTarget;
                    break;
                }
            }
        }
        return src_state != last_state ? std::optional<api::ResourceBarrierDesc>( api::ResourceBarrierDesc { resource.get_view().get_resource(), src_state, dst_state } ) : std::nullopt;
    }
};

}