// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

enum class TaskType {
    ResourceTransition,
    RenderPass,
    ComputePass
};

struct ResourceTransitionTask {
    std::reference_wrapper<Resource> resource;
    api::ResourceState before;
    api::ResourceState after;
};

struct RenderPassTask {
    std::string name;
    std::function<void(RenderPassContext&)> exec_func;

    std::vector<AttachmentDesc> m_attachments;
    std::vector<std::reference_wrapper<Resource>> m_writes;
    std::vector<std::reference_wrapper<Resource>> m_reads;
};

struct ComputePassTask {
    std::string name;
    std::function<void(RenderPassContext&)> exec_func;
};

class Task {
public:

    Task(const ResourceTransitionTask& task) 
        : m_type(TaskType::ResourceTransition), m_task(task) {

    }

    Task(const RenderPassTask& task)
        : m_type(TaskType::RenderPass), m_task(task) {

    }

    Task(const ComputePassTask& task)
        : m_type(TaskType::ComputePass), m_task(task) {

    }

    TaskType get_type() const { return m_type; }

    std::variant<
        ResourceTransitionTask,
        RenderPassTask,
        ComputePassTask
    >& get_task() { return m_task; }

private:

    TaskType m_type;

    std::variant<
        ResourceTransitionTask,
        RenderPassTask,
        ComputePassTask
    > m_task;
};

}