// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

struct RenderPassTask {
    std::function<void(RenderPassContext&)> exec_func;
    std::vector<AttachmentDesc> attachments;
};

struct ComputePassTask {
    std::function<void(ComputePassContext&)> exec_func;
    std::vector<BufferDesc> buffers;
};

class Task {
friend class FrameGraph;
public:

    Task(const TaskType type, const std::string& name, const std::variant<RenderPassTask, ComputePassTask>& task) 
        : m_type(type), m_name(name), m_task(task), m_ref_count(0) {

    }

    TaskType get_type() const { return m_type; }
    const std::string& get_name() const { return m_name; }

    void write(const ResourceHandle& handle, const std::variant<AttachmentDesc, BufferDesc>& desc) {
        switch(m_type) {
            case TaskType::RenderPass: {
                auto& attachment = std::get<AttachmentDesc>(desc);
                auto& render_pass_task = std::get<RenderPassTask>(m_task);
                render_pass_task.attachments.emplace_back(attachment);
                break;
            }
            case TaskType::ComputePass: {
                auto& buffer = std::get<BufferDesc>(desc);
                auto compute_pass_task = std::get<ComputePassTask>(m_task);
                compute_pass_task.buffers.emplace_back(buffer);
                break;
            }
        }
        m_writes.emplace_back(handle);
    }

    void read(const ResourceHandle& handle) {
        m_reads.emplace_back(handle);
    }

private:

    std::string m_name;
    TaskType m_type;

    std::variant<
        RenderPassTask,
        ComputePassTask
    > m_task;

    std::vector<ResourceHandle> m_writes;
    std::vector<ResourceHandle> m_reads;

    uint32 m_ref_count;
};

}