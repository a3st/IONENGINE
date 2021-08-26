// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphResourceType {
    RenderTarget,
    Buffer
};

struct AttachmentDesc {
    Format format;
    RenderPassLoadOp load_op;
    ClearValueColor clear_value;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(format, load_op, clear_value) < std::tie(rhs.format, rhs.load_op, rhs.clear_value);
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

    FrameGraphResource(const FrameGraphResourceType type, const uint64 id) : m_type(type), m_id(id) {

    }

protected:

    uint64 get_id() const { return m_id; }
    FrameGraphResourceType get_type() const { return m_type; }

private:

    FrameGraphResourceType m_type;
    uint64 m_id;
};

class FrameGraphResourceCache {
public:

    FrameGraphResourceCache() {

    }

private:

};

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder(FrameGraphResourceCache& resource_cache) : m_resource_cache(resource_cache) {
        
    }

    FrameGraphResource add_input(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return {  };
    }

    FrameGraphResource add_output(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return { };
    }

protected:

    

private:

    std::reference_wrapper<FrameGraphResourceCache> m_resource_cache;
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

        std::vector<FrameGraphResource> input_resources;
        std::vector<FrameGraphResource> output_resources;

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
        const std::function<void(RenderPassBuilder&, T&)>& setup_pass_func, 
        const std::function<void(RenderPassContext&, const T&)>& exec_pass_func
    ) {
        T pass_data{};

        RenderPassBuilder builder(m_resource_cache);
        setup_pass_func(builder, pass_data);

        FrameGraph::RenderPassDesc render_pass_desc{};
        
        render_pass_desc.exec_func = std::bind(exec_pass_func, std::placeholders::_1, pass_data);

        m_render_passes.emplace_back(render_pass_desc);
    }

    void build() {


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