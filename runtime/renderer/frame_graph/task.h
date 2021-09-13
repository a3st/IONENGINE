// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphTaskType {
    ResourceTransition,
    RenderPass
};

struct ResourceTransitionTask {
    std::reference_wrapper<FrameGraphResource> resource;
    ResourceState before;
    ResourceState after;
};

struct RenderPassTask {
    std::reference_wrapper<FrameGraphRenderPass> render_pass;
};

class FrameGraphTask {
public:

    FrameGraphTask(const ResourceTransitionTask& task) 
        : m_type(FrameGraphTaskType::ResourceTransition), m_task(task) {

    }

    FrameGraphTask(const RenderPassTask& task)
        : m_type(FrameGraphTaskType::RenderPass), m_task(task) {

    }

    FrameGraphTaskType get_type() const { return m_type; }

    std::variant<
        ResourceTransitionTask,
        RenderPassTask
    > get_task() const { return m_task; }

private:

    FrameGraphTaskType m_type;

    std::variant<
        ResourceTransitionTask,
        RenderPassTask
    > m_task;
};

}