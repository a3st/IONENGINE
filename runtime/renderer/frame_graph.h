// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphResourceType {
    RenderTarget
};

struct AttachmentDesc {
    std::reference_wrapper<View> view;
    RenderPassLoadOp load_op;
    ClearValueColor clear_value;
};

class FrameGraphResource {
public:

    FrameGraphResource() {

    }

    FrameGraphResource(const FrameGraphResourceType type, const uint64 id) : m_resource_type(type), m_resource_id(id) {

    }

private:

    FrameGraphResourceType m_resource_type;
    uint64 m_resource_id;
};

class RenderPassResources {
friend class RenderPassBuilder;
friend class FrameGraph;
public:

    RenderPassResources() : m_attachments_offset(0) {
        
    }

private:

    uint64 m_attachments_offset;

    std::map<int64, AttachmentDesc> m_render_pass_attachments;

    uint64 allocate_attachment(View& view, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {

        m_render_pass_attachments.emplace(m_attachments_offset, AttachmentDesc { view, load_op, clear_value });
        m_attachments_offset++;
        return m_attachments_offset;
    }
};

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder(RenderPassResources& resources) : m_render_pass_resources(resources) {
        

    }

    FrameGraphResource add_output(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        uint64 offset = m_render_pass_resources.get().allocate_attachment(m_frame_graph_attachments.at(name), load_op, clear_value);
        return { FrameGraphResourceType::RenderTarget, offset };
    }

private:

    std::reference_wrapper<RenderPassResources> m_render_pass_resources;

    std::map<std::string, std::reference_wrapper<View>> m_frame_graph_attachments;
};

class RenderPassContext {
public:

    RenderPassContext(Device& device) : m_device(device) {

        m_command_list = m_device.get().create_command_list(CommandListType::Graphics);
    }

    CommandList& get_command_list() { return *m_command_list; }

private:

    std::reference_wrapper<Device> m_device;

    std::unique_ptr<CommandList> m_command_list;
};

class FrameGraph {
public:

    FrameGraph(Device& device) : m_device(device)  {
        
        m_render_pass_resources = std::make_unique<RenderPassResources>();
        m_render_pass_builder = std::make_unique<RenderPassBuilder>(*m_render_pass_resources);
        m_render_pass_context = std::make_unique<RenderPassContext>(device);
    }

    void bind_attachment(const std::string& name, View& view) {

        m_render_pass_builder->m_frame_graph_attachments.emplace(name, view);
    }

    template<typename T>
    void add_pass(
        const std::string& name, 
        const std::function<void(RenderPassBuilder&, T&)>& setup_pass_func, 
        const std::function<void(RenderPassResources&, const T&, RenderPassContext&)>& exec_pass_func
    ) {
        T pass_data{};

        setup_pass_func(*m_render_pass_builder, pass_data);

        auto arg_less_func = std::bind(exec_pass_func, std::ref(*m_render_pass_resources), pass_data, std::ref(*m_render_pass_context));
        m_render_pass_execs.emplace_back(name, arg_less_func);
    }

    void build() {

        if(m_render_passes.empty()) {
            
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
        }
    }

    void execute() {

        ClearValueDesc clear_value_desc = {
            { { m_render_pass_resources->m_render_pass_attachments.begin()->second.clear_value } }
        };

        m_render_pass_context->get_command_list().resource_barriers({ { m_render_pass_resources->m_render_pass_attachments.begin()->second.view.get().get_resource(), ResourceState::Present, ResourceState::RenderTarget } });
        m_render_pass_context->get_command_list().begin_render_pass(*m_render_passes.back(), *m_frame_buffers.back(), clear_value_desc);
        m_render_pass_execs.back().second();
        m_render_pass_context->get_command_list().end_render_pass();
        m_render_pass_context->get_command_list().resource_barriers({ { m_render_pass_resources->m_render_pass_attachments.begin()->second.view.get().get_resource(), ResourceState::RenderTarget, ResourceState::Present } });
        m_render_pass_context->get_command_list().close();

        m_device.get().get_command_queue(CommandListType::Graphics).execute_command_lists({ { m_render_pass_context->get_command_list() } });

        //m_render_pass_context->get_command_list().reset();
    }

private:

    std::reference_wrapper<Device> m_device;

    std::unique_ptr<RenderPassBuilder> m_render_pass_builder;
    std::unique_ptr<RenderPassResources> m_render_pass_resources;
    std::unique_ptr<RenderPassContext> m_render_pass_context;

    std::vector<std::pair<std::string, std::function<void()>>> m_render_pass_execs;

    std::vector<std::unique_ptr<RenderPass>> m_render_passes;
    std::vector<std::unique_ptr<FrameBuffer>> m_frame_buffers;
};

}