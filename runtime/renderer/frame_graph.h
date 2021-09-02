// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphResourceType {
    RenderTarget,
    Buffer
};

struct AttachmentDesc {
    RenderPassLoadOp load_op;
    ClearValueColor clear_value;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(load_op, clear_value) < std::tie(rhs.load_op, rhs.clear_value);
    }
};

struct BufferDesc {

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

    FrameGraphResource(const FrameGraphResourceType type, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) : m_type(type) {

        m_type = type;
        
    }

protected:

    FrameGraphResourceType get_type() const { return m_type; }

private:

    FrameGraphResourceType m_type;

    std::variant<
        AttachmentDesc,
        BufferDesc
    > m_resource_desc;
};

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder() {
        
    }

    FrameGraphResource add_input(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return {  };
    }

    FrameGraphResource add_output(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return { };
    }

    FrameGraphResource add_output(const std::string& name, const RenderPassLoadOp load_op) {
        return { };
    }

protected:

    const std::vector<FrameGraphResource>& get_input_resources() const { return m_input_resources; }
    const std::vector<FrameGraphResource>& get_output_resources() const { return m_output_resources; }

private:

    std::vector<FrameGraphResource> m_input_resources;
    std::vector<FrameGraphResource> m_output_resources;

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

    FrameGraphResourceCache m_resource_cache;

    std::vector<FrameGraph::RenderPassDesc> m_render_passes;
    std::vector<FrameGraph::ComputePassDesc> m_compute_passes;
    std::map<std::string, std::reference_wrapper<View>> m_bind_resources;
};

}