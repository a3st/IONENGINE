// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphResourceType {
    RenderTarget
};

struct AttachmentDesc {
    Format format;
    RenderPassLoadOp load_op;
    ClearValueColor clear_value;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(format, load_op, clear_value) < std::tie(rhs.format, rhs.load_op, rhs.clear_value);
    }
};

struct RenderPassKey {
    std::vector<AttachmentDesc> color_attachments;
    AttachmentDesc depth_stencil_attachment;

    bool operator<(const RenderPassKey& rhs) {
        return std::tie(color_attachments, depth_stencil_attachment) < std::tie(rhs.color_attachments, rhs.depth_stencil_attachment);
    }
};

class FrameGraphResource {
public:

    FrameGraphResource() {

    }

    FrameGraphResource(const FrameGraphResourceType type, const uint64 offset) : m_resource_type(type), m_resource_offset(offset) {

    }

private:

    FrameGraphResourceType m_resource_type;
    uint64 m_resource_offset;
};

class FrameGraphBuilder {
friend class FrameGraph;
public:

    FrameGraphBuilder() {
        
    }

    FrameGraphResource add_input(const std::string& name) {
        return {  };
    }

    FrameGraphResource add_output(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return { };
    }

private:

    std::vector<AttachmentDesc> color_attachments;

    const std::vector<AttachmentDesc>& get_color_attachments() const {
        return color_attachments;
    }
};

class RenderPassContext {
public:

    RenderPassContext(CommandList& command_list) : m_command_list(command_list) {

    }

    CommandList& get_command_list() { return m_command_list; }

private:

    std::reference_wrapper<CommandList> m_command_list;
};

class FrameGraph {
public:

    struct RenderPassDesc {
        
        std::vector<AttachmentDesc> color_attachments;
        AttachmentDesc depth_attachment;

        std::vector<FrameGraphResource> color_resources;

        std::function<void(RenderPassContext&)> exec_func;
    };

    struct ComputePassDesc {
        
    };

    enum class TaskType {
        RenderPass,
        ComputePass
    };

    struct TaskDesc {
        TaskType type;
        uint32 index;
    };

    FrameGraph(Device& device) : m_device(device)  {
        
    }

    void bind_attachment(const std::string& name, View& view) {

        m_bind_attachments.emplace(name, view);
    }

    template<typename T>
    void add_pass(
        const std::string& name, 
        const std::function<void(FrameGraphBuilder&, T&)>& setup_pass_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_pass_func
    ) {
        T pass_data{};

        FrameGraphBuilder builder;
        setup_pass_func(builder, pass_data);
        
        FrameGraph::RenderPassDesc render_pass_desc{};
        render_pass_desc.color_attachments = builder.get_color_attachments();
        render_pass_desc.exec_func = std::bind(exec_pass_func, std::placeholders::_1, pass_data);

        m_render_passes.emplace_back(render_pass_desc);

        FrameGraph::TaskDesc task_desc{};
        task_desc.type = FrameGraph::TaskType::RenderPass;
        task_desc.index = static_cast<uint32>(m_render_passes.size());

        m_tasks.emplace_back(task_desc);
    }

    /*void build() {

        for(auto& resource : m_frame_graph_builder.get()->m_frame_graph_resources) {

        }
            
        auto& resource = m_render_pass_resources->m_render_pass_attachments.begin()->second.view.get().get_resource();

        RenderPassColorDesc render_pass_color_desc = {
            resource.get_format(),
            m_render_pass_resources->m_render_pass_attachments.begin()->second.load_op
        };

        RenderPassDesc render_pass_desc = {
            { { render_pass_color_desc } }
        };

        m_render_passes.emplace_back(m_device.get().create_render_pass(render_pass_desc));
            
        FrameBufferDesc frame_buffer_desc = {
            *m_render_passes.back(),
            800,
            600,
            { { m_render_pass_resources->m_render_pass_attachments.begin()->second.view } }
        };

        m_frame_buffers.emplace_back(m_device.get().create_frame_buffer(frame_buffer_desc));

        std::cout << "Render pass created" << std::endl;
    }*/

    void execute(CommandList& command_list) {



        /*ClearValueDesc clear_value_desc = {
            { { m_render_pass_resources->m_render_pass_attachments.begin()->second.clear_value } }
        };

        m_render_pass_context->get_command_list().resource_barriers({ { m_render_pass_resources->m_render_pass_attachments.begin()->second.view.get().get_resource(), ResourceState::Present, ResourceState::RenderTarget } });
        m_render_pass_context->get_command_list().begin_render_pass(*m_render_passes.back(), *m_frame_buffers.back(), clear_value_desc);
        m_render_pass_execs.back().second();
        m_render_pass_context->get_command_list().end_render_pass();
        m_render_pass_context->get_command_list().resource_barriers({ { m_render_pass_resources->m_render_pass_attachments.begin()->second.view.get().get_resource(), ResourceState::RenderTarget, ResourceState::Present } });
        m_render_pass_context->get_command_list().close();

        m_device.get().get_command_queue(CommandListType::Graphics).execute_command_lists({ { m_render_pass_context->get_command_list() } });

        m_render_pass_context->get_command_list().reset();*/
    }

private:

    std::reference_wrapper<Device> m_device;

    std::unique_ptr<FrameGraphBuilder> m_frame_graph_builder;

    //std::map<RenderPassKey, std::unique_ptr<RenderPass>> m_render_passes;

    std::map<std::string, std::reference_wrapper<View>> m_bind_attachments;

    std::vector<FrameGraph::TaskDesc> m_tasks;

    std::vector<FrameGraph::RenderPassDesc> m_render_passes;
    std::vector<FrameGraph::ComputePassDesc> m_compute_passes;
};

}