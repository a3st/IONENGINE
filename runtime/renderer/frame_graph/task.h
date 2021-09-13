// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphTaskType {
    ResourceTransition,
    RenderPass,
    ComputePass
};

struct ResourceTransitionTask {
    std::reference_wrapper<FrameGraphResource> resource;
    ResourceState before;
    ResourceState after;
};

struct RenderPassTask {
    std::string name;
    std::function<void(RenderPassContext&)> exec_func;

    std::vector<AttachmentDesc> m_attachments;
    std::vector<std::reference_wrapper<FrameGraphResource>> m_writes;
    std::vector<std::reference_wrapper<FrameGraphResource>> m_reads;
};

struct ComputePassTask {

};

class FrameGraphTask {
public:

    FrameGraphTask(const ResourceTransitionTask& task) 
        : m_type(FrameGraphTaskType::ResourceTransition), m_task(task) {

    }

    FrameGraphTask(const RenderPassTask& task)
        : m_type(FrameGraphTaskType::RenderPass), m_task(task) {

    }

    FrameGraphTask(const ComputePassTask& task)
        : m_type(FrameGraphTaskType::ComputePass), m_task(task) {

    }

    FrameGraphTaskType get_type() const { return m_type; }

    std::variant<
        ResourceTransitionTask,
        RenderPassTask,
        ComputePassTask
    > get_task() const { return m_task; }

private:

    FrameGraphTaskType m_type;

    std::variant<
        ResourceTransitionTask,
        RenderPassTask,
        ComputePassTask
    > m_task;
};

}